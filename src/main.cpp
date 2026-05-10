#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include "engine/application.h"
#include "game/game.h"

// Small entrypoint. Engine owns SDL setup; game owns gameplay.
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Create the game and run it inside the engine loop.
    idlegolem::game::Game game;
    idlegolem::engine::Application application;
    if (!application.Run(game)) {
        SDL_Log("Application failed to run.");
        return 1;
    }

    return 0;
}
