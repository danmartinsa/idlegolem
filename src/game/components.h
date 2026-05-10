#pragma once

#include <SDL3/SDL.h>

#include <glm/vec2.hpp>

namespace idlegolem::game {

// ECS data shared by gameplay and rendering systems.

// Coarse worker role used for setup and role-specific behavior.
enum class WorkerRole { Zombie, Skeleton };

// High-level animation state chosen by gameplay code.
enum class AnimationState { Idle, Walk, Dig };

// Tracks frame timing for a looping sprite-sheet animation.
class Animation {
    int frameCount = 1;
    float frameDuration = 1.0f;
    float elapsed = 0.0f;
    int frame = 0;

   public:
    Animation() = default;

    Animation(int frameCount, float frameDuration)
        : frameCount(frameCount > 0 ? frameCount : 1),
          frameDuration(frameDuration > 0.0f ? frameDuration : 1.0f) {}

    [[nodiscard]] int CurrentFrame() const { return frame; }

    void Step(float deltaTime) {
        elapsed += deltaTime;
        while (elapsed >= frameDuration) {
            elapsed -= frameDuration;
            frame = (frame + 1) % frameCount;
        }
    }

    void Reset() {
        elapsed = 0.0f;
        frame = 0;
    }
};

// Marks an entity as a shared worker and stores its role.
struct Worker {
    WorkerRole role = WorkerRole::Zombie;
};

// World position in pixels.
struct Transform {
    float x = 0.0f;
    float y = 0.0f;
};

// Movement speed and direction in pixels per second.
struct Velocity {
    glm::vec2 value{0.0f, 0.0f};
};

// Persistent horizontal facing for sprite flips.
struct Facing {
    bool isLeft = true;
};

// Left and right bounds for simple patrol movement.
struct PatrolBounds {
    float minX = 0.0f;
    float maxX = 0.0f;
};

// Sprite size plus simple fallback tint data.
struct Renderable {
    float width = 128.0f;
    float height = 128.0f;
    SDL_Color primaryColor{255, 255, 255, 255};
    SDL_Color accentColor{96, 96, 96, 255};
};

// One texture-backed animation clip.
struct AnimationClip {
    SDL_Texture* texture = nullptr;
    Animation animation{};
    float frameWidth = 0.0f;
    float frameHeight = 0.0f;
};

// Sprite data for one worker plus the active animation state.
struct Sprite {
    AnimationState state = AnimationState::Idle;
    AnimationClip idle{};
    AnimationClip walk{};
    AnimationClip dig{};

    AnimationClip& CurrentClip() {
        switch (state) {
            case AnimationState::Idle:
                return idle;
            case AnimationState::Walk:
                return walk;
            case AnimationState::Dig:
                return dig;
        }

        return idle;
    }

    const AnimationClip& CurrentClip() const {
        switch (state) {
            case AnimationState::Idle:
                return idle;
            case AnimationState::Walk:
                return walk;
            case AnimationState::Dig:
                return dig;
        }

        return idle;
    }

    void SetState(AnimationState newState) {
        if (state == newState) {
            return;
        }

        // Start the new clip from its first frame.
        state = newState;
        CurrentClip().animation.Reset();
    }
};

// Extra state for the dig/pause loop used by some worker roles.
struct DigBehavior {
    float digCooldownRemaining = 1.8f;
    float digDurationRemaining = 0.0f;
    float storedVelocityX = -32.0f;
};

// Spinning projectile state for thrown bones.
struct BoneProjectile {
    float gravity = 720.0f;
    float rotationDegrees = 0.0f;
    float angularVelocityDegrees = 540.0f;
};

}  // namespace idlegolem::game
