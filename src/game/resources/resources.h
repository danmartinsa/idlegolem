#pragma once

#include <SDL3/SDL.h>

#include <vector>

namespace idlegolem::game {

// Texture cache for the current runtime session
class Resources {
   public:
    // declaation of implcit copy/move members
    Resources() = default;
    Resources(const Resources&) = default;
    Resources(Resources&&) = delete;
    Resources& operator=(const Resources&) = default;
    Resources& operator=(Resources&&) = delete;
    ~Resources();

    // class properties
    std::vector<SDL_Texture*> textures;
    SDL_Texture* zombieIdle = nullptr;
    SDL_Texture* zombieWalk = nullptr;
    SDL_Texture* bone = nullptr;
    SDL_Texture* tower = nullptr;
    SDL_Texture* cross = nullptr;

    // Free loaded texture and reset cached pointer
    void DestroyAssets();

    // Load a PNG texture and keep nearest neighbor scaling
    [[nodiscard]] SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* path) const;

    // Load the actor sprite sheets to be used by scene
    void LoadAssets(SDL_Renderer* renderer);
};

}  // namespace idlegolem::game
