#include "game/game.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace {

// Local gameplay tuning values.
constexpr float kSpawnMarginX = 24.0f;
constexpr float kSpawnMarginY = 32.0f;
constexpr float kZombieSpeed = -54.0f;
constexpr float kSkeletonSpeed = -32.0f;
constexpr float kSkeletonDigCooldownSeconds = 1.8f;
constexpr float kSkeletonDigDurationSeconds = 0.75f;
constexpr float kGroundTopOffset = 86.0f;
constexpr float kBoneRenderSize = 32.0f;
constexpr float kBoneSpawnX = 96.0f;
constexpr float kBoneFlightMinSeconds = 0.45f;
constexpr float kBoneFlightMaxSeconds = 1.0f;
constexpr float kBoneFlightDistanceScale = 480.0f;
constexpr float kBoneSourceSize = 16.0f;
constexpr float kUiPadding = 16.0f;
constexpr float kUiIconSize = 24.0f;
constexpr float kUiTextGap = 12.0f;
constexpr float kUiCounterFontSize = 20.0f;
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr float kMaxDeltaSeconds = 0.05f;
constexpr const char* kWindowTitle = "Idle Golem";
constexpr SDL_Color kUiPanelColor{22, 20, 18, 220};
constexpr SDL_Color kUiPanelBorderColor{82, 74, 66, 255};
constexpr SDL_Color kUiDigitColor{236, 228, 214, 255};
constexpr const char* kCounterFontPaths[] = {
    "assets/ui.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/liberation-sans-fonts/LiberationSans-Regular.ttf",
    "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
};

// Fully assembled data used by the shared worker spawn path.
struct WorkerDefinition {
    idlegolem::game::Renderable renderable;
    idlegolem::game::Velocity velocity;
    idlegolem::game::Sprite sprite;
    bool hasDigBehavior = false;
    float storedVelocityX = 0.0f;
};

[[nodiscard]] idlegolem::game::AnimationClip MakeClip(SDL_Texture* texture, int frameCount,
                                                      float frameDuration, float frameWidth,
                                                      float frameHeight) {
    return idlegolem::game::AnimationClip{
        texture,
        idlegolem::game::Animation{frameCount, frameDuration},
        frameWidth,
        frameHeight,
    };
}

[[nodiscard]] WorkerDefinition BuildWorkerDefinition(const idlegolem::game::WorkerRole role,
                                                     const idlegolem::game::Resources& resources) {
    WorkerDefinition definition{};

    // Translate the worker role into visuals, movement, and optional behavior.
    switch (role) {
        case idlegolem::game::WorkerRole::Zombie:
            definition.renderable = idlegolem::game::Renderable{
                32.0f, 32.0f, SDL_Color{124, 182, 110, 255}, SDL_Color{54, 70, 48, 255}};
            definition.velocity = idlegolem::game::Velocity{glm::vec2{kZombieSpeed, 0.0f}};
            definition.sprite.state = idlegolem::game::AnimationState::Walk;
            definition.sprite.idle = MakeClip(resources.zombieIdle, 6, 0.16f, 22.0f, 18.0f);
            definition.sprite.walk = MakeClip(resources.zombieWalk, 8, 0.10f, 21.0f, 19.0f);
            definition.sprite.dig = MakeClip(resources.zombieIdle, 6, 0.20f, 22.0f, 18.0f);
            return definition;
        case idlegolem::game::WorkerRole::Skeleton:
            definition.renderable = idlegolem::game::Renderable{
                32.0f, 32.0f, SDL_Color{226, 222, 208, 255}, SDL_Color{86, 72, 64, 255}};
            definition.velocity = idlegolem::game::Velocity{glm::vec2{kSkeletonSpeed, 0.0f}};
            definition.sprite.state = idlegolem::game::AnimationState::Walk;
            definition.sprite.idle = MakeClip(resources.skeletonIdle, 6, 0.18f, 11.0f, 15.0f);
            definition.sprite.walk = MakeClip(resources.skeletonWalk, 6, 0.14f, 13.0f, 15.0f);
            definition.sprite.dig = MakeClip(resources.skeletonIdle, 6, 0.12f, 11.0f, 15.0f);
            definition.hasDigBehavior = true;
            definition.storedVelocityX = kSkeletonSpeed;
            return definition;
    }

    return definition;
}

}  // namespace

namespace idlegolem::game {

Game::Game() {
    config_.windowWidth = kWindowWidth;
    config_.windowHeight = kWindowHeight;
    config_.maxDeltaSeconds = kMaxDeltaSeconds;
    config_.windowTitle = kWindowTitle;
}

const engine::ApplicationConfig& Game::Config() const { return config_; }

bool Game::Initialize(SDL_Renderer* renderer) {
    registry_.clear();
    bonesThrown_ = 0;
    counterDirty_ = true;

    if (!TTF_Init()) {
        SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
        return false;
    }
    ttfInitialized_ = true;

    if (!LoadCounterFont()) {
        SDL_Log("Failed to load a font for the bone counter.");
        TTF_Quit();
        ttfInitialized_ = false;
        return false;
    }

    resources_.LoadAssets(renderer);
    RefreshCounterTexture(renderer);

    // Seed the scene so something is visible on startup.
    SpawnZombie(220.0f, 320.0f);
    SpawnSkeleton(820.0f, 320.0f);
    return true;
}

void Game::Shutdown() {
    // Drop entities before releasing textures referenced by their clips.
    registry_.clear();
    DestroyCounterTexture();

    if (counterFont_ != nullptr) {
        TTF_CloseFont(counterFont_);
        counterFont_ = nullptr;
    }

    resources_.DestroyAssets();

    if (ttfInitialized_) {
        TTF_Quit();
        ttfInitialized_ = false;
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
            SpawnBone(spawnX, spawnY);
            break;
        case SDL_BUTTON_RIGHT:
            SpawnSkeleton(spawnX, spawnY);
            break;
        default:
            break;
    }
}

void Game::Update(const float deltaTime) {
    const float clampedDelta = std::clamp(deltaTime, 0.0F, config_.maxDeltaSeconds);

    // Order matters: behavior changes velocity, movement applies it, then
    // animation state and clip playback catch up.
    UpdateBones(clampedDelta);
    UpdateWorkerDigging(clampedDelta);
    UpdatePatrol(clampedDelta);
    UpdateLocomotion();
    UpdateAnimation(clampedDelta);
}

void Game::Render(SDL_Renderer* renderer) const {
    // Draw a simple background first, then render the actors on top.
    SDL_SetRenderDrawColor(renderer, 18, 18, 24, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 42, 38, 34, 255);
    const SDL_FRect ground{0.0f, static_cast<float>(config_.windowHeight) - 84.0f,
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

    RenderWorkers(renderer);
    RenderBones(renderer);
    RenderBoneCounter(renderer);
}

void Game::SpawnBone(const float targetX, const float targetY) {
    if (resources_.bone == nullptr) {
        SDL_Log("Bone projectile skipped: bone texture is not loaded.");
        return;
    }

    const float groundTop = static_cast<float>(config_.windowHeight) - kGroundTopOffset;
    const float startX = kBoneSpawnX;
    const float startY = groundTop - kBoneRenderSize;
    const float flightTime =
        std::clamp(kBoneFlightMinSeconds + std::abs(targetX - startX) / kBoneFlightDistanceScale,
                   kBoneFlightMinSeconds, kBoneFlightMaxSeconds);
    const float gravity = 720.0f;
    const float velocityX = (targetX - startX) / flightTime;
    const float velocityY =
        (targetY - startY - 0.5f * gravity * flightTime * flightTime) / flightTime;

    const entt::entity entity = registry_.create();
    registry_.emplace<Transform>(entity, Transform{startX, startY});
    registry_.emplace<Velocity>(entity, Velocity{glm::vec2{velocityX, velocityY}});
    registry_.emplace<Renderable>(
        entity, Renderable{kBoneRenderSize, kBoneRenderSize, SDL_Color{255, 255, 255, 255},
                           SDL_Color{255, 255, 255, 255}});
    registry_.emplace<BoneProjectile>(entity);
    ++bonesThrown_;
    counterDirty_ = true;
}

void Game::SpawnZombie(const float x, const float y) { SpawnWorker(WorkerRole::Zombie, x, y); }

void Game::SpawnSkeleton(const float x, const float y) { SpawnWorker(WorkerRole::Skeleton, x, y); }

void Game::SpawnWorker(const WorkerRole role, float x, float y) {
    const WorkerDefinition definition = BuildWorkerDefinition(role, resources_);

    // Clamp mouse spawns into the playable strip.
    x = std::clamp(
        x, kSpawnMarginX,
        static_cast<float>(config_.windowWidth) - definition.renderable.width - kSpawnMarginX);
    y = std::clamp(
        y, kSpawnMarginY,
        static_cast<float>(config_.windowHeight) - definition.renderable.height - kSpawnMarginY);

    const entt::entity entity = registry_.create();

    // Every worker gets the shared movement, render, and animation components.
    registry_.emplace<Worker>(entity, role);
    registry_.emplace<Transform>(entity, Transform{x, y});
    registry_.emplace<Velocity>(entity, definition.velocity);
    registry_.emplace<Facing>(entity, Facing{definition.velocity.value.x <= 0.0f});
    registry_.emplace<PatrolBounds>(
        entity, PatrolBounds{kSpawnMarginX, static_cast<float>(config_.windowWidth) -
                                                definition.renderable.width - kSpawnMarginX});
    registry_.emplace<Renderable>(entity, definition.renderable);
    registry_.emplace<Sprite>(entity, definition.sprite);

    if (definition.hasDigBehavior) {
        registry_.emplace<DigBehavior>(entity, DigBehavior{kSkeletonDigCooldownSeconds, 0.0f,
                                                           definition.storedVelocityX});
    }
}

void Game::UpdateBones(const float deltaTime) {
    auto view = registry_.view<BoneProjectile, Transform, Velocity, Renderable>();
    std::vector<entt::entity> expiredEntities;

    // Bones spin while gravity bends the path.
    for (const entt::entity entity : view) {
        BoneProjectile& bone = view.get<BoneProjectile>(entity);
        Transform& transform = view.get<Transform>(entity);
        Velocity& velocity = view.get<Velocity>(entity);
        const Renderable& renderable = view.get<Renderable>(entity);

        transform.x += velocity.value.x * deltaTime;
        transform.y += velocity.value.y * deltaTime;
        velocity.value.y += bone.gravity * deltaTime;
        bone.rotationDegrees =
            std::fmod(bone.rotationDegrees + bone.angularVelocityDegrees * deltaTime, 360.0f);

        if (transform.x + renderable.width < 0.0f ||
            transform.x > static_cast<float>(config_.windowWidth) ||
            transform.y > static_cast<float>(config_.windowHeight)) {
            expiredEntities.push_back(entity);
        }
    }

    if (!expiredEntities.empty()) {
        registry_.destroy(expiredEntities.begin(), expiredEntities.end());
    }
}

void Game::UpdateWorkerDigging(const float deltaTime) {
    auto view = registry_.view<DigBehavior, Velocity, Sprite>();

    // Dig-capable workers run a custom walk -> dig -> walk loop.
    for (const entt::entity entity : view) {
        DigBehavior& behavior = view.get<DigBehavior>(entity);
        Velocity& velocity = view.get<Velocity>(entity);
        Sprite& sprite = view.get<Sprite>(entity);

        if (behavior.digDurationRemaining > 0.0f) {
            behavior.digDurationRemaining =
                std::max(0.0f, behavior.digDurationRemaining - deltaTime);
            velocity.value.x = 0.0f;
            sprite.SetState(AnimationState::Dig);

            if (behavior.digDurationRemaining == 0.0f) {
                // Resume the stored patrol direction after digging.
                velocity.value.x = behavior.storedVelocityX;
                behavior.digCooldownRemaining = kSkeletonDigCooldownSeconds;
                sprite.SetState(AnimationState::Walk);
            }

            continue;
        }

        behavior.digCooldownRemaining -= deltaTime;
        if (behavior.digCooldownRemaining <= 0.0f) {
            // Preserve the current direction before stopping to dig.
            behavior.storedVelocityX = velocity.value.x == 0.0f ? kSkeletonSpeed : velocity.value.x;
            velocity.value.x = 0.0f;
            behavior.digDurationRemaining = kSkeletonDigDurationSeconds;
            behavior.digCooldownRemaining = 0.0f;
            sprite.SetState(AnimationState::Dig);
            continue;
        }

        if (velocity.value.x == 0.0f) {
            velocity.value.x = behavior.storedVelocityX;
        }

        sprite.SetState(AnimationState::Walk);
    }
}

void Game::UpdatePatrol(const float deltaTime) {
    auto view = registry_.view<Transform, Velocity, Facing, PatrolBounds>();

    // Apply movement, then flip direction when patrol bounds are hit.
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
            }
        } else if (transform.x > patrolBounds.maxX) {
            transform.x = patrolBounds.maxX;
            if (velocity.value.x > 0.0f) {
                velocity.value.x = -velocity.value.x;
            }
        }

        if (velocity.value.x < 0.0f) {
            facing.isLeft = true;
        } else if (velocity.value.x > 0.0f) {
            facing.isLeft = false;
        }
    }
}

void Game::UpdateLocomotion() {
    auto view = registry_.view<Worker, Velocity, Sprite>(entt::exclude<DigBehavior>);

    // Most workers use a simple idle/walk rule based on horizontal speed.
    for (const entt::entity entity : view) {
        const Velocity& velocity = view.get<Velocity>(entity);
        Sprite& sprite = view.get<Sprite>(entity);

        // Dig-capable workers are excluded because their own system sets Dig/Walk.
        sprite.SetState(velocity.value.x == 0.0f ? AnimationState::Idle : AnimationState::Walk);
    }
}

void Game::UpdateAnimation(const float deltaTime) {
    auto view = registry_.view<Sprite>();

    // Advance whichever clip the gameplay systems selected this frame.
    for (const entt::entity entity : view) {
        Sprite& sprite = view.get<Sprite>(entity);
        sprite.CurrentClip().animation.Step(deltaTime);
    }
}

void Game::RenderWorkers(SDL_Renderer* renderer) const {
    auto view = registry_.view<Worker, Transform, Renderable, Sprite, Facing>();

    // Resolve the active clip, sample its current frame, and draw it.
    for (const entt::entity entity : view) {
        const Transform& transform = view.get<Transform>(entity);
        const Renderable& renderable = view.get<Renderable>(entity);
        const Sprite& sprite = view.get<Sprite>(entity);
        const Facing& facing = view.get<Facing>(entity);
        const AnimationClip& clip = sprite.CurrentClip();

        if (clip.texture == nullptr || clip.frameWidth <= 0.0f || clip.frameHeight <= 0.0f) {
            continue;
        }

        // Read one frame from the sheet and stretch it into world space.
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

void Game::RenderBones(SDL_Renderer* renderer) const {
    if (resources_.bone == nullptr) {
        return;
    }

    auto view = registry_.view<BoneProjectile, Transform, Renderable>();
    const SDL_FRect src{0.0f, 0.0f, kBoneSourceSize, kBoneSourceSize};

    // Draw the bone with rotation around its center.
    for (const entt::entity entity : view) {
        const BoneProjectile& bone = view.get<BoneProjectile>(entity);
        const Transform& transform = view.get<Transform>(entity);
        const Renderable& renderable = view.get<Renderable>(entity);
        const SDL_FRect dst{transform.x, transform.y, renderable.width, renderable.height};
        const SDL_FPoint center{renderable.width * 0.5f, renderable.height * 0.5f};
        SDL_RenderTextureRotated(renderer, resources_.bone, &src, &dst, bone.rotationDegrees,
                                 &center, SDL_FLIP_NONE);
    }
}

void Game::RenderBoneCounter(SDL_Renderer* renderer) const {
    Game* game = const_cast<Game*>(this);
    game->RefreshCounterTexture(renderer);

    const float textWidth = boneCounterTextTexture_ != nullptr ? boneCounterTextWidth_ : 0.0f;
    const float textHeight = boneCounterTextTexture_ != nullptr ? boneCounterTextHeight_ : 0.0f;
    const float panelWidth = kUiPadding * 2.0f + kUiIconSize + kUiTextGap + textWidth;
    const float panelHeight = kUiPadding * 2.0f + kUiIconSize;
    const SDL_FRect panel{12.0f, 12.0f, panelWidth, panelHeight};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, kUiPanelColor.r, kUiPanelColor.g, kUiPanelColor.b,
                           kUiPanelColor.a);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, kUiPanelBorderColor.r, kUiPanelBorderColor.g,
                           kUiPanelBorderColor.b, kUiPanelBorderColor.a);
    SDL_RenderRect(renderer, &panel);

    if (resources_.bone != nullptr) {
        const SDL_FRect src{0.0f, 0.0f, kBoneSourceSize, kBoneSourceSize};
        const SDL_FRect dst{panel.x + kUiPadding, panel.y + kUiPadding, kUiIconSize, kUiIconSize};
        SDL_RenderTexture(renderer, resources_.bone, &src, &dst);
    }

    if (boneCounterTextTexture_ != nullptr) {
        const SDL_FRect textRect{
            panel.x + kUiPadding + kUiIconSize + kUiTextGap,
            panel.y + kUiPadding + (kUiIconSize - textHeight) * 0.5f,
            textWidth,
            textHeight,
        };
        SDL_RenderTexture(renderer, boneCounterTextTexture_, nullptr, &textRect);
    }
}

bool Game::LoadCounterFont() {
    for (const char* path : kCounterFontPaths) {
        TTF_Font* font = TTF_OpenFont(path, kUiCounterFontSize);
        if (font != nullptr) {
            counterFont_ = font;
            return true;
        }
    }

    SDL_Log("Bone counter font load failed. Tried %zu paths. Last error: %s",
            std::size(kCounterFontPaths), SDL_GetError());
    return false;
}

void Game::RefreshCounterTexture(SDL_Renderer* renderer) {
    if (!counterDirty_ || renderer == nullptr || counterFont_ == nullptr) {
        return;
    }

    DestroyCounterTexture();

    const std::string text = std::to_string(std::max(0, bonesThrown_));
    SDL_Surface* surface = TTF_RenderText_Blended(counterFont_, text.c_str(), 0, kUiDigitColor);
    if (surface == nullptr) {
        SDL_Log("Bone counter text render failed: %s", SDL_GetError());
        return;
    }

    boneCounterTextTexture_ = SDL_CreateTextureFromSurface(renderer, surface);
    if (boneCounterTextTexture_ == nullptr) {
        SDL_Log("Bone counter texture creation failed: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return;
    }

    boneCounterTextWidth_ = static_cast<float>(surface->w);
    boneCounterTextHeight_ = static_cast<float>(surface->h);
    SDL_DestroySurface(surface);
    counterDirty_ = false;
}

void Game::DestroyCounterTexture() {
    if (boneCounterTextTexture_ != nullptr) {
        SDL_DestroyTexture(boneCounterTextTexture_);
        boneCounterTextTexture_ = nullptr;
    }

    boneCounterTextWidth_ = 0.0f;
    boneCounterTextHeight_ = 0.0f;
}

}  // namespace idlegolem::game
