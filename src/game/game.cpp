#include "game/game.h"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

#include <algorithm>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "entt/entity/fwd.hpp"
#include "game/components.h"

namespace {
// Local gameplay tuning values.
constexpr float kSpawnMarginX = 24.0f;
constexpr float kSpawnMarginY = 32.0f;
constexpr float kZombieSpeed = -54.0f;
constexpr float kGroundTopOffset = 86.0f;
// Cross Definition
constexpr float kCrossRenderSize = 8.0f;
// Renderer Options
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr float kMaxDeltaSeconds = 0.05f;
constexpr const char* kWindowTitle = "Idle Golem";

// Fully assembled data used by the shared spawn path.
struct ActorDefinition {
    idlegolem::game::Renderable renderable;
    idlegolem::game::Velocity velocity;
    idlegolem::game::SpriteSet spriteSet;
};

[[nodiscard]] idlegolem::game::SpriteClip MakeClip(SDL_Texture* texture, int frameCount,
                                                   float frameDuration, float frameWidth,
                                                   float frameHeight) {
    return idlegolem::game::SpriteClip{
        texture,
        idlegolem::game::Animation{frameCount, frameDuration},
        frameWidth,
        frameHeight,
    };
}

[[nodiscard]] ActorDefinition BuildActorDefinition(const idlegolem::game::ActorKind kind,
                                                   const idlegolem::game::Resources& resources) {
    ActorDefinition definition{};

    switch (kind) {
        case idlegolem::game::ActorKind::Zombie:
            definition.renderable = idlegolem::game::Renderable{
                32.0f, 32.0f, SDL_Color{124, 182, 110, 255}, SDL_Color{54, 70, 48, 255}};
            definition.velocity = idlegolem::game::Velocity{glm::vec2{kZombieSpeed, 0.0f}};
            definition.spriteSet.state = idlegolem::game::AnimationState::Walk;
            definition.spriteSet.idle = MakeClip(resources.zombieIdle, 6, 0.16f, 22.0f, 18.0f);
            definition.spriteSet.walk = MakeClip(resources.zombieWalk, 8, 0.10f, 21.0f, 19.0f);
            return definition;
    }

    return definition;
}

}  // namespace

namespace idlegolem::game {
Game::Game() : config_{kWindowWidth, kWindowHeight, kMaxDeltaSeconds, kWindowTitle} {}

const engine::ApplicationConfig& Game::Config() const { return config_; }

bool Game::Initialize(SDL_Renderer* renderer) {
    registry_.clear();

    resources_.LoadAssets(renderer);

    RenderTower(renderer, resources_.tower);
    return true;
}

void Game::Shutdown() {
    registry_.clear();
    resources_.DestroyAssets();
}

void Game::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        // Mouse buttons act as quick spawn controls.
        const float spawnX = event.button.x;
        const float spawnY = event.button.y;

        switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                SpawnActor(ActorKind::Zombie, spawnX, spawnY);
                break;
            case SDL_BUTTON_RIGHT:
                SpawnCross();
                break;
            default:
                break;
        }
    }
}

void Game::Update(const float deltaTime) {
    const float clampedDelta = std::clamp(deltaTime, 0.0F, config_.maxDeltaSeconds);

    // Order matters: behavior changes velocity, movement applies it, then
    // animation state and clip playback catch up.
    // UpdateBones(clampedDelta);
    UpdatePatrol(clampedDelta);
    UpdateVelocity();
    UpdateAnimation(clampedDelta);
}

void Game::Render(SDL_Renderer* renderer) const {
    // Draw a simple background first, then render the actors on top.
    SDL_SetRenderDrawColor(renderer, 18, 18, 24, 255);
    SDL_RenderClear(renderer);

    RenderActors(renderer);
    RenderTower(renderer, resources_.tower);
    RenderCross(renderer, resources_.cross);
    // RenderBones(renderer);
    // RenderBoneCounter(renderer);
}

void Game::SpawnActor(const ActorKind kind, float x, float y) {
    const ActorDefinition definition = BuildActorDefinition(kind, resources_);

    // Clamp mouse spawns into the playable strip
    x = std::clamp(
        x, kSpawnMarginX,
        static_cast<float>(config_.windowWidth) - definition.renderable.width - kSpawnMarginX);
    y = std::clamp(
        y, kSpawnMarginY,
        static_cast<float>(config_.windowHeight) - definition.renderable.height - kSpawnMarginY);

    const entt::entity entity = registry_.create();

    registry_.emplace<Actor>(entity, kind);
    registry_.emplace<Transform>(entity, Transform{x, y});
    registry_.emplace<Velocity>(entity, definition.velocity);
    registry_.emplace<Facing>(entity, Facing{definition.velocity.value.x <= 0.0f});
    registry_.emplace<PatrolBounds>(
        entity, PatrolBounds{kSpawnMarginX, static_cast<float>(config_.windowWidth) -
                                                definition.renderable.width - kSpawnMarginX});
    registry_.emplace<Renderable>(entity, definition.renderable);
    registry_.emplace<SpriteSet>(entity, definition.spriteSet);
}

void Game::UpdatePatrol(const float deltaTime) {
    auto view = registry_.view<Transform, Velocity, Facing, PatrolBounds>();

    // Apply movement, then flip direction when patrol bound are hit
    for (const entt::entity entity : view) {
        Transform& transform = view.get<Transform>(entity);
        Velocity& velocity = view.get<Velocity>(entity);
        Facing& facing = view.get<Facing>(entity);
        const PatrolBounds& patrolBounds = view.get<PatrolBounds>(entity);

        transform.x += velocity.value.x * deltaTime;
        transform.y += velocity.value.y * deltaTime;

        if (transform.x < patrolBounds.minX) {
            transform.x = patrolBounds.minX;
            if (velocity.value.x < 0.0f) {
                velocity.value.x = -velocity.value.x;
                facing.isLeft = false;
            }
        } else if (transform.x > patrolBounds.maxX) {
            transform.x = patrolBounds.maxX;
            if (velocity.value.x > 0.0f) {
                velocity.value.x = -velocity.value.x;
                facing.isLeft = true;
            }
        }
    }
};

void Game::UpdateVelocity() {
    auto view = registry_.view<Actor, Velocity, SpriteSet>();

    for (const entt::entity entity : view) {
        const Velocity& velocity = view.get<Velocity>(entity);
        SpriteSet& spriteSet = view.get<SpriteSet>(entity);

        spriteSet.SetState(velocity.value.x == 0.0f ? AnimationState::Idle : AnimationState::Walk);
    }
}

void Game::UpdateAnimation(const float deltaTime) {
    auto view = registry_.view<SpriteSet>();

    // Advance clip the gameplay system selected this frame
    for (const entt::entity entity : view) {
        SpriteSet& spriteSet = view.get<SpriteSet>(entity);
        spriteSet.CurrentClip().animation.Step(deltaTime);
    }
}

void Game::RenderTower(SDL_Renderer* renderer, SDL_Texture* tex) const {
    const SDL_FRect src{
        0.0f,
        0.0f,
        48.0f,
        128.0f,
    };

    const SDL_FRect dst{
        200.0f,
        200.0f,
        48.0f,
        128.0f,
    };

    SDL_RenderTexture(renderer, tex, &src, &dst);
}

void Game::SpawnCross() {
    if (resources_.cross == nullptr) {
        SDL_Log("Cross texture is not loaded.");
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(100.f, 200.f);
    std::uniform_real_distribution<float> disY(500.f, 510.f);

    float posX = disX(gen);
    float posY = disY(gen);

    const entt::entity entity = registry_.create();
    registry_.emplace<Transform>(entity, Transform{posX, posY});
    registry_.emplace<Renderable>(
        entity, Renderable{kCrossRenderSize, kCrossRenderSize, SDL_Color{255, 255, 255, 255},
                           SDL_Color{255, 255, 255, 255}});
    registry_.emplace<Cross>(entity);
}

void Game::RenderCross(SDL_Renderer* renderer, SDL_Texture* tex) const {
    auto view = registry_.view<Cross, Transform, Renderable>();

    for (const entt::entity entity : view){
        const Transform& transform = view.get<Transform>(entity);
        const Renderable& renderable = view.get<Renderable>(entity);

    const SDL_FRect src{
        0.0f,
        0.0f,
        kCrossRenderSize,
        kCrossRenderSize,
    };

    const SDL_FRect dst{
        transform.x,
        transform.y,
        renderable.width,
        renderable.height
    };

    SDL_RenderTexture(renderer, tex, &src, &dst);
    }

}

void Game::RenderActors(SDL_Renderer* renderer) const {
    auto view = registry_.view<Actor, Transform, Renderable, SpriteSet, Facing>();

    for (const entt::entity entity : view) {
        const Transform& transform = view.get<Transform>(entity);
        const Renderable& renderable = view.get<Renderable>(entity);
        const SpriteSet& spriteSet = view.get<SpriteSet>(entity);
        const Facing& facing = view.get<Facing>(entity);
        const SpriteClip& clip = spriteSet.CurrentClip();

        if (clip.texture == nullptr || clip.frameWidth <= 0.0f || clip.frameHeight <= 0.0f) {
            continue;
        }

        // Read one frame from the sheet and fit (stretch) it intioo the world space
        const SDL_FRect src{
            static_cast<float>(clip.animation.CurrentFrame()) * clip.frameWidth,
            0.0f,
            clip.frameWidth,
            clip.frameHeight,
        };

        const SDL_FRect dst{transform.x, transform.y, renderable.width, renderable.height};

        SDL_RenderTextureRotated(renderer, clip.texture, &src, &dst, 0.0, nullptr,
                                 facing.isLeft ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    }
}

}  // namespace idlegolem::game
