#pragma once

namespace idlegolem::engine {

// Plain application settings passed from the game into the engine loop.
struct ApplicationConfig {
    int windowWidth;
    int windowHeight;
    float maxDeltaSeconds;
    const char* windowTitle;
};

}  // namespace idlegolem::engine
