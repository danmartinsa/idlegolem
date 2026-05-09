#pragma once

namespace idlegolem::game {
class Game;
}

namespace idlegolem::engine {

// Engine-facing application shell. Handles SDL setup, the frame loop, and
// teardown without owning game rules.
class Application {
   public:
    // Run one game instance through the SDL application lifecycle.
    [[nodiscard]] bool Run(game::Game& game) const;
};

}  // namespace idlegolem::engine
