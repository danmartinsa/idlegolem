#include "engine/application.h"

#include <SDL3/SDL.h>

namespace idlegolem::engine {

bool Application::Run(GameInterface& game) const {
    // Keep SDL startup and shutdown in one place.
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

    // Poll input, advance the game, then present the frame.
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else {
                game.HandleEvent(event);
            }
        }

        const Uint64 currentTicks = SDL_GetTicks();
        const float deltaTime = static_cast<float>(currentTicks - lastTicks) / 1000.0F;
        lastTicks = currentTicks;

        // Game code updates simulation and records draw calls; engine presents.
        game.Update(deltaTime);
        game.Render(renderer);
        SDL_RenderPresent(renderer);
    }

    // Let game-owned resources release before SDL objects go away.
    game.Shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
}

}  // namespace idlegolem::engine
