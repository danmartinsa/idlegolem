#pragma once

#include <SDL3/SDL.h>

#include <functional>
#include <memory>
#include <string>

#include "engine/game.h"

namespace engine {

struct ApplicationConfig {
    int windowWidth = 1280;
    int windowHeight = 720;
    std::string windowTitle;
};

using GameFactory =
    std::function<std::unique_ptr<Game>(SDL_Renderer* renderer)>;

int RunApplication(const ApplicationConfig& config, const GameFactory& factory);

}  // namespace engine
