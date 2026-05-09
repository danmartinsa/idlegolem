#include "game/resources.h"

namespace {

// Keep raw asset paths local to the resource loader.
constexpr const char* kZombieIdlePath =
    "assets/pppack/Enemies/Zombie_Axe/Zombie_Axe_Side-left_Idle-Sheet6.png";
constexpr const char* kZombieWalkPath =
    "assets/pppack/Enemies/Zombie_Axe/Zombie_Axe_Side-left_Walk-Sheet8.png";
constexpr const char* kSkeletonIdlePath =
    "assets/pppack/Enemies/Zombie_Small/Zombie_Small_Side-left_Idle-Sheet6.png";
constexpr const char* kSkeletonWalkPath =
    "assets/pppack/Enemies/Zombie_Small/Zombie_Small_Side-left_Walk-Sheet6.png";

}  // namespace

namespace idlegolem::game {

Resources::~Resources() { DestroyAssets(); }

void Resources::DestroyAssets() {
    // Track all loaded textures in one place for simple cleanup.
    for (SDL_Texture* texture : textures) {
        SDL_DestroyTexture(texture);
    }

    textures.clear();
    zombieIdle = nullptr;
    zombieWalk = nullptr;
    skeletonIdle = nullptr;
    skeletonWalk = nullptr;
}

SDL_Texture* Resources::LoadTexture(SDL_Renderer* renderer,
                                    const char* path) const {
    if (renderer == nullptr) {
        SDL_Log("LoadTexture failed for %s: renderer is null", path);
        return nullptr;
    }

    SDL_Surface* surface = SDL_LoadPNG(path);
    if (surface == nullptr) {
        SDL_Log("SDL_LoadPNG failed for %s: %s", path, SDL_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (texture == nullptr) {
        SDL_Log("SDL_CreateTextureFromSurface failed for %s: %s", path,
                SDL_GetError());
        return nullptr;
    }

    // Preserve sharp pixel edges when the sprite is scaled up.
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return texture;
}

void Resources::LoadAssets(SDL_Renderer* renderer) {
    // Rebuild the cache from scratch on each load.
    DestroyAssets();

    zombieIdle = LoadTexture(renderer, kZombieIdlePath);
    if (zombieIdle != nullptr) {
        textures.push_back(zombieIdle);
    }

    zombieWalk = LoadTexture(renderer, kZombieWalkPath);
    if (zombieWalk != nullptr) {
        textures.push_back(zombieWalk);
    }

    skeletonIdle = LoadTexture(renderer, kSkeletonIdlePath);
    if (skeletonIdle != nullptr) {
        textures.push_back(skeletonIdle);
    }

    skeletonWalk = LoadTexture(renderer, kSkeletonWalkPath);
    if (skeletonWalk != nullptr) {
        textures.push_back(skeletonWalk);
    }
}

}  // namespace idlegolem::game
