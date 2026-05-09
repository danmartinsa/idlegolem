#pragma once

#include <SDL3/SDL.h>

namespace idlegolem::engine {

struct ApplicationConfig {
    //default values
    // int windowWidth = 1280;
    // int windowHeight = 720;
    // float maxDeltaSeconds = 0.05f;
    // const char* windowTitle = "Idle Golem";
    int windowWidth;
    int windowHeight;
    float maxDeltaSeconds;
    const char* windowTitle;
};

// Engine-side contract for any game run by Application.
class GameInterface {
   public:
    virtual ~GameInterface() = default;

    virtual const ApplicationConfig& Config() const = 0;
    virtual bool Initialize(SDL_Renderer* renderer) = 0;
    virtual void Shutdown() = 0;
    virtual void HandleEvent(const SDL_Event& event) = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer) const = 0;
};

// Engine-facing application shell. Handles SDL setup, the frame loop, and
// teardown without owning game rules.
class Application {
   public:
    // Run one game instance through the SDL application lifecycle.
    [[nodiscard]] bool Run(GameInterface& game) const;
};

}  // namespace idlegolem::engine
