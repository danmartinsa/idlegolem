#pragma once

namespace idlegolem::engine {

// Central configuration shared by the engine-owned application loop and the
// concrete game implementation.
struct ApplicationConfig {
    int windowWidth = 1280;
    int windowHeight = 720;
    float maxDeltaSeconds = 0.05f;
    const char* windowTitle = "Idle Golem";
};

}  // namespace idlegolem::engine
