#include "engine/sdl_application.h"

namespace engine {

int RunApplication(const ApplicationConfig& config, const GameFactory& factory) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window =
        SDL_CreateWindow(config.windowTitle.c_str(), config.windowWidth, config.windowHeight, 0);
    if (window == nullptr) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    std::unique_ptr<Game> game = factory(renderer);
    if (game == nullptr) {
        SDL_Log("Game factory returned null");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowTitle(window, game->WindowTitle().c_str());

    bool isRunning = true;
    Uint64 lastTicks = SDL_GetTicks();

    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            } else {
                game->HandleEvent(event);
            }
        }

        const Uint64 currentTicks = SDL_GetTicks();
        const float deltaSeconds = static_cast<float>(currentTicks - lastTicks) / 1000.0F;
        lastTicks = currentTicks;

        game->Update(deltaSeconds);
        SDL_SetWindowTitle(window, game->WindowTitle().c_str());

        game->Render();
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    game.reset();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

}  // namespace engine
