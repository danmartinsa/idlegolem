#pragma once

#include <SDL3/SDL.h>

#include <glm/vec2.hpp>

namespace idlegolem::game {

// ECS data shared by gameplay and rendering systems.

// Coarse actor family used for setup and type-specific behavior.
enum class ActorKind { Zombie, Skeleton };

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

// Stores the actor family on an entity.
struct Actor {
    ActorKind kind = ActorKind::Zombie;
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
struct SpriteClip {
    SDL_Texture* texture = nullptr;
    Animation animation{};
    float frameWidth = 0.0f;
    float frameHeight = 0.0f;
};

// All clips for one actor plus the active state.
struct AnimationSet {
    AnimationState state = AnimationState::Idle;
    SpriteClip idle{};
    SpriteClip walk{};
    SpriteClip dig{};

    SpriteClip& CurrentClip() {
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

    const SpriteClip& CurrentClip() const {
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

// Extra state for the skeleton dig/pause loop.
struct SkeletonBehavior {
    float digCooldownRemaining = 1.8f;
    float digDurationRemaining = 0.0f;
    float storedVelocityX = -32.0f;
};

}  // namespace idlegolem::game
