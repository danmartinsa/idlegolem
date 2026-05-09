#pragma once

#include <SDL3/SDL.h>

#include <entt/entt.hpp>

#include "engine/application_config.h"
#include "game/components.h"
#include "game/resources.h"

namespace idlegolem::game {

// Concrete game layer. Owns ECS state, resources, simulation, and rendering.
class Game {
   public:
    Game() = default;
    ~Game() = default;

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Window and timing config consumed by the engine layer.
    [[nodiscard]] const engine::ApplicationConfig& Config() const;

    // Load runtime resources and seed the first scene.
    [[nodiscard]] bool Initialize(SDL_Renderer* renderer);

    // Clear ECS state and release runtime resources.
    void Shutdown();

    // Translate SDL input into gameplay actions.
    void HandleEvent(const SDL_Event& event);

    // Advance one simulation step for the current frame.
    void Update(float deltaTime);

    // Record the current frame's draw commands.
    void Render(SDL_Renderer* renderer) const;

   private:
    // Convenience wrappers over the shared spawn path.
    void SpawnZombie(float x, float y);
    void SpawnSkeleton(float x, float y);
    void SpawnActor(ActorKind kind, float x, float y);

    // Ordered simulation stages.
    void UpdateSkeletons(float deltaTime);
    void UpdatePatrol(float deltaTime);
    void UpdateLocomotion();
    void UpdateAnimation(float deltaTime);

    // Draw all actor sprites after the scene background.
    void RenderActors(SDL_Renderer* renderer) const;

    engine::ApplicationConfig config_{};
    Resources resources_{};
    entt::registry registry_{};
};

}  // namespace idlegolem::game
