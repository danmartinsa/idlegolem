#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

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
    // Input and spawning.
    void SpawnBone(float targetX, float targetY);
    void SpawnZombie(float x, float y);
    void SpawnSkeleton(float x, float y);
    void SpawnActor(ActorKind kind, float x, float y);

    // Simulation.
    void UpdateBones(float deltaTime);
    void UpdateSkeletons(float deltaTime);
    void UpdatePatrol(float deltaTime);
    void UpdateLocomotion();
    void UpdateAnimation(float deltaTime);

    // Rendering.
    void RenderActors(SDL_Renderer* renderer) const;
    void RenderBones(SDL_Renderer* renderer) const;
    void RenderBoneCounter(SDL_Renderer* renderer) const;

    // UI text resources.
    bool LoadCounterFont();
    void RefreshCounterTexture(SDL_Renderer* renderer);
    void DestroyCounterTexture();

    // Core state.
    entt::registry registry_{};
    engine::ApplicationConfig config_{};
    Resources resources_{};

    // Counter UI state.
    TTF_Font* counterFont_ = nullptr;
    SDL_Texture* boneCounterTextTexture_ = nullptr;
    float boneCounterTextWidth_ = 0.0f;
    float boneCounterTextHeight_ = 0.0f;

    // Gameplay state.
    int bonesThrown_ = 0;
    bool counterDirty_ = true;
    bool ttfInitialized_ = false;
};

}  // namespace idlegolem::game
