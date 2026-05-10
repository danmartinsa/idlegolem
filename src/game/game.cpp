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
constexpr float kGroundTopOffset = 86.0f;
// Renderer Options
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr float kMaxDeltaSeconds = 0.05f;
constexpr const char* kWindowTitle = "Idle Golem";

// Fully assembled data used by the shared spawn path.
struct ActorDefinition {
    idlegolem::game::Renderable renderable;
    idlegolem::game::Velocity velocity;
    idlegolem::game::AnimationSet animationSet;
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

    [[nodiscard]] ActorDefinition BuildActorDefinition(
        const idlegolem::game::ActorKind kind, const idlegolem::game::Resources& resources) {
        ActorDefinition definition{};

        switch (kind) {
            case idlegolem::game::ActorKind::Zombie:
                definition.renderable = idlegolem::game::Renderable{
                    32.0f, 32.0f, SDL_Color{124, 182, 110, 255}, SDL_Color{54, 70, 48, 255}};
                definition.velocity = idlegolem::game::Velocity{glm::vec2{kZombieSpeed, 0.0f}};
                definition.animationSet.state = idlegolem::game::AnimationState::Walk;
                definition.animationSet.idle =
                    MakeClip(resources.zombieIdle, 6, 0.16f, 22.0f, 18.0f);
                definition.animationSet.walk =
                    MakeClip(resources.zombieWalk, 8, 0.10f, 21.0f, 19.0f);
                definition.animationSet.dig =
                    MakeClip(resources.zombieIdle, 6, 0.20f, 22.0f, 18.0f);
                return definition;
        }
    }
}

}  // namespace

namespace idlegolem::game {
Game::Game() : config_{kWindowWidth, kWindowHeight, kMaxDeltaSeconds, kWindowTitle} {}

const engine::ApplicationConfig& Game::Config() const { return config_; }

bool Game::Initialize(SDL_Renderer* renderer) {
    registry_.clear();

    resources_.LoadAssets(renderer);
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
                SpawnWorker(spawnX, spawnY);
                break;
            case SDL_BUTTON_RIGHT:
                break;
            default:
                break;
        }
    }
}

}  // namespace idlegolem::game
