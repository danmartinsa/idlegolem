#include "engine/application.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>

namespace idlegolem::engine {

bool Application::Run(GameInterface& game) const {
    // SDL Startup and lifetime manage
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    const ApplicationConfig& config = game.Config();
    SDL_Window* window =
        SDL_CreateWindow(config.windowTitle, config.windowWidth, config.windowHeight, 0);

    if (window == nullptr) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (!game.Initialize(renderer)) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    bool running = true;
    Uint64 lastTicks = SDL_GetTicks();

    // Poll input, advance the game and present the frame
    // since SDL is double buffer
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else {
                game.HandleEvent(event);
            }
        }
    }

    // Delta Time calculation
    const Uint64 currentTicks = SDL_GetTicks();
    const float deltaTime = static_cast<float>(currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;

    // Game code updates simulation ond records draw calls;
    // Then presents
    game.Update(deltaTime);
    game.Render(renderer);
    SDL_RenderPresent(renderer);

    return true;
}

}  // namespace idlegolem::engine
