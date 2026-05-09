#pragma once

#include <SDL3/SDL.h>

#include <glm/vec2.hpp>

namespace idlegolem::game {

// ECS data shared

// Coarse actor family used for setup and type-specific behavior.
enum class ActorKind { Zombie };

// High-level animation state chosen by gameplay code.
enum class AnimationState { Idle, Walk };

class Animation {
    int frameCount = 1;
    float frameDuration = 1.0f;
    float elapsed = 0.0f;
    int frame = 0;

   public:
    Animation() = default;

    Animation(int frameCount, float frameDuration)
        : frameCount(frameCount > 0 ? frameCount : 1),
          frameDuration(frameDuration > 0.0f ? frameDuration : 1.0f) {};

    [[nodiscard]] int CurrentFrame() const { return frame; }
};

}  // namespace idlegolem::game
