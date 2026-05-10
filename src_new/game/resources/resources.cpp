#include "game/resources/resources.h"

namespace {

// Keep raw assets path local to the resource loader.

constexpr const char* kZombieIdlePath =
    "assets/pppack/Enemies/Zombie_Axe/Zombie_Axe_Side-left_Idle-Sheet6.png";
constexpr const char* kZombieWalkPath =
    "assets/pppack/Enemies/Zombie_Axe/Zombie_Axe_Side-left_Walk-Sheet8.png";
constexpr const char* kBonePath = "assets/bone.png";

}  // namespace

namespace idlegolem::game {

Resources::~Resources() { DestroyAssets(); }

void Resources::DestroyAssets() {
    // Track all loaded texture in on place
    for (SDL_Texture* texture : textures) {
        SDL_DestroyTexture(texture);
    }

    textures.clear();
    zombieIdle = nullptr;
    zombieWalk = nullptr;
    bone = nullptr;
};

// Load images from PNGs
// SDL_Surface is pixel data the CPU can access.
// SDL_Texture is pixel data the GPU can access.
SDL_Texture* Resources::LoadTexture(SDL_Renderer* renderer, const char* path) const {
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
        SDL_Log("SDL_CreateTextureFromSurface failed for %s: %s", path, SDL_GetError());
        return nullptr;
    }

    // Preserve sharp pixel edges when the sprite is scaled up.
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return texture;
}

void Resources::LoadAssets(SDL_Renderer* renderer) {
    // Rebuild the cache from scratch on each load
    DestroyAssets();

    zombieIdle = LoadTexture(renderer, kZombieIdlePath);
    if (zombieIdle != nullptr) {
        textures.push_back(zombieIdle);
    }

    zombieWalk = LoadTexture(renderer, kZombieWalkPath);
    if (zombieWalk != nullptr) {
        textures.push_back(zombieWalk);
    }

    bone = LoadTexture(renderer, kBonePath);
    if (bone != nullptr) {
        textures.push_back(bone);
    }
};

}  // namespace idlegolem::game
