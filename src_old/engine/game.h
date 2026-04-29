#pragma once

#include <SDL3/SDL.h>

#include <string>

namespace engine {

class Game {
   public:
    virtual ~Game() = default;

    virtual void HandleEvent(const SDL_Event& event) = 0;
    virtual void Update(float deltaSeconds) = 0;
    virtual void Render() const = 0;
    [[nodiscard]] virtual const std::string& WindowTitle() const = 0;
};

}  // namespace engine
