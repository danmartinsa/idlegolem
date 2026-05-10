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
    Game::Game() : config_{1920, 1080, 0.05f, "Ck"} {}

    bool Game::Initialize(SDL_Renderer* renderer) {
        registry_.clear();

        resources_.LoadAssets(renderer);
        return true;
    }

    void Game::ShutDown() {
        registry_.clear()
    }

} //namespace idlegolem::game
