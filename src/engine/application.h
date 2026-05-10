#pragma once

#include "engine/game_interface.h"

namespace idlegolem::engine {

// Engine-facing application shell. Handles SDL setup, the frame loop, and
// teardown without owning game rules.
class Application {
   public:
    // Run one game instance through the SDL application lifecycle.
    [[nodiscard]] bool Run(GameInterface& game) const;
};

}  // namespace idlegolem::engine
