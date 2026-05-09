#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <entt/entt.hpp>

#include "engine/application.h"

namespace idlegolem::game {

// Game layer, owns ECS, resources, simulation and rendering.
class Game : public engine::GameInterface {
   public:
    Game() = default;
    ~Game() override = default;

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Window and timing config consumed by the engine layer.
    [[nodiscard]] const engine::ApplicationConfig& Config() const override;

    // Load runtime resources and seed the first scene.
    [[nodiscard]] bool Initialize(SDL_Renderer* renderer) override;

    // Clear ECS state and release runtime resources.
    void Shutdown() override;

    // Translate SDL input into gameplay actions.
    void HandleEvent(const SDL_Event& event) override;

    // Advance one simulation step for the current frame.
    void Update(float deltaTime) override;

    // Record the current frame's draw commands.
    void Render(SDL_Renderer* renderer) const override;

   private:
    // Input and Spawn
    void SpawnZombie(float x, float y);
    void SpawnActor(ActorKind kind, float x, float y)
};

}  // namespace idlegolem::game
