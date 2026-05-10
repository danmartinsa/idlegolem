#pragma once

#include <SDL3/SDL.h>

#include <vector>

namespace idlegolem::game {

// Texture cache for the current runtime session.
class Resources {
   public:
    std::vector<SDL_Texture*> textures;
    SDL_Texture* zombieIdle = nullptr;
    SDL_Texture* zombieWalk = nullptr;
    SDL_Texture* skeletonIdle = nullptr;
    SDL_Texture* skeletonWalk = nullptr;
    SDL_Texture* bone = nullptr;

    ~Resources();

    // Free loaded textures and reset cached pointers.
    void DestroyAssets();

    // Load a PNG texture and keep nearest-neighbor scaling.
    [[nodiscard]] SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* path) const;

    // Load the actor sprite sheets used by the scene.
    void LoadAssets(SDL_Renderer* renderer);
};

}  // namespace idlegolem::game
