#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <entt/entt.hpp>

#include "engine/application.h"
#include "game/components.h"
#include "game/resources/resources.h"

namespace idlegolem::game {

// Game layer, owns ECS, resources, simulation and rendering.
class Game : public engine::GameInterface {
   public:
    Game();
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
    void SpawnBone(float targetX, float targetY);
    void SpawnActor(ActorKind kind, float x, float y);

    // Simulation
    void UpdateBones(float deltaTime);
    void UpdateAnimation(float deltaTime);
    void UpdatePatrol(float deltaTime);
    void UpdateVelocity();

    // Rendering
    void RenderActors(SDL_Renderer* renderer) const;
    void RenderBones(SDL_Renderer* renderer) const;

    // Core state
    entt::registry registry_{};
    engine::ApplicationConfig config_{};
    Resources resources_{};

    // Counter UI State
    TTF_Font* counterFount_ = nullptr;
    SDL_Texture* boneCounterTextTexture_ = nullptr;
    float boneCounterTextWidth_ = 0.0f;
    float boneCounterTextHeight_ = 0.0f;

    // Gameplay state.
    int bonesThrown_ = 0;
    bool counterDirty_ = true;
    bool ttfInitialized_ = false;
};

}  // namespace idlegolem::game
