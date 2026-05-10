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

// Stores Actor family on an Entity
struct Actor {
    ActorKind kind = ActorKind::Zombie;
};

// World position in pixels
struct Transform {
    float x = 0.0f;
    float y = 0.0f;
};

// Movement speed and direction in pixels per sec.
struct Velocity {
    glm::vec2 value{0.0f, 0.0f};
};

// Entity direction for sprite flip
struct Facing {
    bool isLeft = true;
};


// Sprite size and simple fallback tint data
struct Renderable {
    float width = 128.0f;
    float height = 128.0f;
    SDL_Color primaryColor{255, 255, 255, 255};
    SDL_Color accentColor{96, 96, 96, 255};
};

// One Texture-backed animation clip.
struct SpriteClip {
    SDL_Texture* texture = nullptr;
    Animation animation{};
    float frameWidth = 0.0f;
    float frameHeight = 0.0f;
};

struct AnimationSet {
    AnimationState state = AnimationState::Idle;
    SpriteClip idle{};
    SpriteClip walk{};

    SpriteClip& CurrentClip() {
        switch (state) {
            case AnimationState::Idle:
                return idle;
            case AnimationState::Walk:
                return walk;
        }

        return idle;
    }

    const SpriteClip& CurrentClip() const {
        switch (state) {
            case AnimationState::Idle:
                return idle;
            case AnimationState::Walk:
                return walk;
        }

        return idle;
    }

    void SetState(AnimationState newState)  {
        if (state == newState) {
            return;
        }

        // Start the new clip from its first frame
        state = newState;
        CurrentClip().animation.Reset();
    }

};

}  // namespace idlegolem::game
