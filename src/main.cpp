#define SDL_MAIN_HANDLED
#include <memory>

#include "engine/sdl_application.h"
#include "game/fleshgolem_game.h"

namespace {
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr const char* kWindowTitle = "idlegolem | scavenging for parts";
}  // namespace

int main(int, char**) {
    const engine::ApplicationConfig config{
        .windowWidth = kWindowWidth,
        .windowHeight = kWindowHeight,
        .windowTitle = kWindowTitle,
    };

    return engine::RunApplication(
        config, [](SDL_Renderer* renderer) { return std::make_unique<FleshgolemGame>(renderer); });
}
