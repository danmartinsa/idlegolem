#include "game/game.h"

#include <algorithm>

namespace {

// Local gameplay tuning values.
constexpr float kSpawnMarginX = 24.0f;
constexpr float kSpawnMarginY = 32.0f;
constexpr float kZombieSpeed = -54.0f;
constexpr float kSkeletonSpeed = -32.0f;
constexpr float kSkeletonDigCooldownSeconds = 1.8f;
constexpr float kSkeletonDigDurationSeconds = 0.75f;

// Fully assembled data used by the shared spawn path.
struct ActorDefinition {
    idlegolem::game::Renderable renderable;
    idlegolem::game::Velocity velocity;
    idlegolem::game::AnimationSet animationSet;
    bool hasSkeletonBehavior = false;
    float skeletonStoredVelocityX = 0.0f;
};

[[nodiscard]] idlegolem::game::SpriteClip MakeClip(
    SDL_Texture* texture, int frameCount, float frameDuration, float frameWidth,
    float frameHeight) {
    return idlegolem::game::SpriteClip{
        texture,
        idlegolem::game::Animation{frameCount, frameDuration},
        frameWidth,
        frameHeight,
    };
}

[[nodiscard]] ActorDefinition BuildActorDefinition(
    const idlegolem::game::ActorKind kind,
    const idlegolem::game::Resources& resources) {
    ActorDefinition definition{};

    // Translate the actor kind into visuals, movement, and optional behavior.
    switch (kind) {
        case idlegolem::game::ActorKind::Zombie:
            definition.renderable =
                idlegolem::game::Renderable{144.0f, 144.0f,
                                            SDL_Color{124, 182, 110, 255},
                                            SDL_Color{54, 70, 48, 255}};
            definition.velocity =
                idlegolem::game::Velocity{glm::vec2{kZombieSpeed, 0.0f}};
            definition.animationSet.state =
                idlegolem::game::AnimationState::Walk;
            definition.animationSet.idle =
                MakeClip(resources.zombieIdle, 6, 0.16f, 22.0f, 18.0f);
            definition.animationSet.walk =
                MakeClip(resources.zombieWalk, 8, 0.10f, 21.0f, 19.0f);
            definition.animationSet.dig =
                MakeClip(resources.zombieIdle, 6, 0.20f, 22.0f, 18.0f);
            return definition;
        case idlegolem::game::ActorKind::Skeleton:
            definition.renderable =
                idlegolem::game::Renderable{104.0f, 144.0f,
                                            SDL_Color{226, 222, 208, 255},
                                            SDL_Color{86, 72, 64, 255}};
            definition.velocity =
                idlegolem::game::Velocity{glm::vec2{kSkeletonSpeed, 0.0f}};
            definition.animationSet.state =
                idlegolem::game::AnimationState::Walk;
            definition.animationSet.idle =
                MakeClip(resources.skeletonIdle, 6, 0.18f, 11.0f, 15.0f);
            definition.animationSet.walk =
                MakeClip(resources.skeletonWalk, 6, 0.14f, 13.0f, 15.0f);
            definition.animationSet.dig =
                MakeClip(resources.skeletonIdle, 6, 0.12f, 11.0f, 15.0f);
            definition.hasSkeletonBehavior = true;
            definition.skeletonStoredVelocityX = kSkeletonSpeed;
            return definition;
    }

    return definition;
}

}  // namespace

namespace idlegolem::game {

const engine::ApplicationConfig& Game::Config() const { return config_; }

bool Game::Initialize(SDL_Renderer* renderer) {
    registry_.clear();
    resources_.LoadAssets(renderer);

    // Seed the scene so something is visible on startup.
    SpawnZombie(220.0f, 320.0f);
    SpawnSkeleton(820.0f, 320.0f);
    return true;
}

void Game::Shutdown() {
    // Drop entities before releasing textures referenced by their clips.
    registry_.clear();
    resources_.DestroyAssets();
}

void Game::SpawnZombie(const float x, const float y) {
    SpawnActor(ActorKind::Zombie, x, y);
}

void Game::SpawnSkeleton(const float x, const float y) {
    SpawnActor(ActorKind::Skeleton, x, y);
}

void Game::SpawnActor(const ActorKind kind, float x, float y) {
    const ActorDefinition definition = BuildActorDefinition(kind, resources_);

    // Clamp mouse spawns into the playable strip.
    x = std::clamp(x, kSpawnMarginX,
                   static_cast<float>(config_.windowWidth) -
                       definition.renderable.width - kSpawnMarginX);
    y = std::clamp(y, kSpawnMarginY,
                   static_cast<float>(config_.windowHeight) -
                       definition.renderable.height - kSpawnMarginY);

    const entt::entity entity = registry_.create();

    // Every actor gets the shared movement, render, and animation components.
    registry_.emplace<Actor>(entity, kind);
    registry_.emplace<Transform>(entity, Transform{x, y});
    registry_.emplace<Velocity>(entity, definition.velocity);
    registry_.emplace<PatrolBounds>(
        entity,
        PatrolBounds{kSpawnMarginX, static_cast<float>(config_.windowWidth) -
                                         definition.renderable.width -
                                         kSpawnMarginX});
    registry_.emplace<Renderable>(entity, definition.renderable);
    registry_.emplace<AnimationSet>(entity, definition.animationSet);

    if (definition.hasSkeletonBehavior) {
        registry_.emplace<SkeletonBehavior>(
            entity,
            SkeletonBehavior{kSkeletonDigCooldownSeconds, 0.0f,
                             definition.skeletonStoredVelocityX});
    }
}

void Game::HandleEvent(const SDL_Event& event) {
    if (event.type != SDL_EVENT_MOUSE_BUTTON_DOWN) {
        return;
    }

    // Mouse buttons act as quick spawn controls.
    const float spawnX = event.button.x;
    const float spawnY = event.button.y;

    switch (event.button.button) {
        case SDL_BUTTON_LEFT:
            SpawnZombie(spawnX, spawnY);
            break;
        case SDL_BUTTON_RIGHT:
            SpawnSkeleton(spawnX, spawnY);
            break;
        default:
            break;
    }
}

void Game::UpdateSkeletons(const float deltaTime) {
    auto view = registry_.view<SkeletonBehavior, Velocity, AnimationSet>();

    // Skeletons run a custom walk -> dig -> walk loop.
    for (const entt::entity entity : view) {
        SkeletonBehavior& behavior = view.get<SkeletonBehavior>(entity);
        Velocity& velocity = view.get<Velocity>(entity);
        AnimationSet& animationSet = view.get<AnimationSet>(entity);

        if (behavior.digDurationRemaining > 0.0f) {
            behavior.digDurationRemaining =
                std::max(0.0f, behavior.digDurationRemaining - deltaTime);
            velocity.value.x = 0.0f;
            animationSet.SetState(AnimationState::Dig);

            if (behavior.digDurationRemaining == 0.0f) {
                // Resume the stored patrol direction after digging.
                velocity.value.x = behavior.storedVelocityX;
                behavior.digCooldownRemaining = kSkeletonDigCooldownSeconds;
                animationSet.SetState(AnimationState::Walk);
            }

            continue;
        }

        behavior.digCooldownRemaining -= deltaTime;
        if (behavior.digCooldownRemaining <= 0.0f) {
            // Preserve the current direction before stopping to dig.
            behavior.storedVelocityX =
                velocity.value.x == 0.0f ? kSkeletonSpeed : velocity.value.x;
            velocity.value.x = 0.0f;
            behavior.digDurationRemaining = kSkeletonDigDurationSeconds;
            behavior.digCooldownRemaining = 0.0f;
            animationSet.SetState(AnimationState::Dig);
            continue;
        }

        if (velocity.value.x == 0.0f) {
            velocity.value.x = behavior.storedVelocityX;
        }

        animationSet.SetState(AnimationState::Walk);
    }
}

void Game::UpdatePatrol(const float deltaTime) {
    auto view = registry_.view<Transform, Velocity, PatrolBounds>();

    // Apply movement, then flip direction when patrol bounds are hit.
    for (const entt::entity entity : view) {
        Transform& transform = view.get<Transform>(entity);
        Velocity& velocity = view.get<Velocity>(entity);
        const PatrolBounds& patrolBounds = view.get<PatrolBounds>(entity);

        transform.x += velocity.value.x * deltaTime;
        transform.y += velocity.value.y * deltaTime;

        if (transform.x < patrolBounds.minX) {
            transform.x = patrolBounds.minX;
            if (velocity.value.x < 0.0f) {
                velocity.value.x = -velocity.value.x;
            }
        } else if (transform.x > patrolBounds.maxX) {
            transform.x = patrolBounds.maxX;
            if (velocity.value.x > 0.0f) {
                velocity.value.x = -velocity.value.x;
            }
        }
    }
}

void Game::UpdateLocomotion() {
    auto view =
        registry_.view<Velocity, AnimationSet>(entt::exclude<SkeletonBehavior>);

    // Most actors use a simple idle/walk rule based on horizontal speed.
    for (const entt::entity entity : view) {
        const Velocity& velocity = view.get<Velocity>(entity);
        AnimationSet& animationSet = view.get<AnimationSet>(entity);

        // Skeletons are excluded because their own system sets Dig/Walk.
        animationSet.SetState(velocity.value.x == 0.0f ? AnimationState::Idle
                                                       : AnimationState::Walk);
    }
}

void Game::UpdateAnimation(const float deltaTime) {
    auto view = registry_.view<AnimationSet>();

    // Advance whichever clip the gameplay systems selected this frame.
    for (const entt::entity entity : view) {
        AnimationSet& animationSet = view.get<AnimationSet>(entity);
        animationSet.CurrentClip().animation.Step(deltaTime);
    }
}

void Game::Update(const float deltaTime) {
    const float clampedDelta =
        std::clamp(deltaTime, 0.0F, config_.maxDeltaSeconds);

    // Order matters: behavior changes velocity, movement applies it, then
    // animation state and clip playback catch up.
    UpdateSkeletons(clampedDelta);
    UpdatePatrol(clampedDelta);
    UpdateLocomotion();
    UpdateAnimation(clampedDelta);
}

void Game::RenderActors(SDL_Renderer* renderer) const {
    auto view = registry_.view<Actor, Transform, Renderable, AnimationSet>();

    // Resolve the active clip, sample its current frame, and draw it.
    for (const entt::entity entity : view) {
        const Transform& transform = view.get<Transform>(entity);
        const Renderable& renderable = view.get<Renderable>(entity);
        const AnimationSet& animationSet = view.get<AnimationSet>(entity);
        const SpriteClip& clip = animationSet.CurrentClip();

        if (clip.texture == nullptr || clip.frameWidth <= 0.0f ||
            clip.frameHeight <= 0.0f) {
            continue;
        }

        // Read one frame from the sheet and stretch it into world space.
        const SDL_FRect src{
            static_cast<float>(clip.animation.CurrentFrame()) * clip.frameWidth,
            0.0f,
            clip.frameWidth,
            clip.frameHeight,
        };
        const SDL_FRect dst{transform.x, transform.y, renderable.width,
                            renderable.height};
        SDL_RenderTexture(renderer, clip.texture, &src, &dst);
    }
}

void Game::Render(SDL_Renderer* renderer) const {
    // Draw a simple background first, then render the actors on top.
    SDL_SetRenderDrawColor(renderer, 18, 18, 24, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 42, 38, 34, 255);
    const SDL_FRect ground{0.0f,
                           static_cast<float>(config_.windowHeight) - 84.0f,
                           static_cast<float>(config_.windowWidth), 84.0f};
    SDL_RenderFillRect(renderer, &ground);

    SDL_SetRenderDrawColor(renderer, 76, 70, 62, 255);
    const SDL_FRect groundLine{
        0.0f,
        static_cast<float>(config_.windowHeight) - 86.0f,
        static_cast<float>(config_.windowWidth),
        4.0f,
    };
    SDL_RenderFillRect(renderer, &groundLine);

    RenderActors(renderer);
}

}  // namespace idlegolem::game
