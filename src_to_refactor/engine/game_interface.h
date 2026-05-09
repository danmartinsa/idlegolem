#pragma once

#include <SDL3/SDL.h>

#include "engine/application_config.h"

namespace idlegolem::engine {

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

}  // namespace idlegolem::engine
