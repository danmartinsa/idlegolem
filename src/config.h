#pragma once

#include <SDL3/SDL.h>

#include <entt/entt.hpp>
#include <vector>

struct ApplicationConfig {
    int windowWidth = 1280;
    int windowHeight = 720;
};

enum ZombieState { Idle, Walk, Dig };

class Resources {
   public:
    std::vector<SDL_Texture *> textures;
    SDL_Texture *zombieIdle = nullptr;
    SDL_Texture *zombieWalk = nullptr;

    void DestroyAssets() {
        if (!textures.empty()) {
            for (SDL_Texture *texture : textures) {
                SDL_DestroyTexture(texture);
            }
        }
    }

    ~Resources() { DestroyAssets(); }

    SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path) {
        SDL_Texture *tex;
        if (!renderer) {
            SDL_Log("loadTexture failed for %s: renderer is null", path);
        }

        SDL_Surface *surface = SDL_LoadPNG(path);
        if (!surface) {
            SDL_Log("SDL_LoadPNG failed for %s: %s", path, SDL_GetError());
        }

        tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (tex == nullptr) {
            SDL_Log("SDL_CreateTextureFromSurface failed for %s: %s", path,
                    SDL_GetError());
        }

        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

        return tex;
    }

    void LoadAssets(SDL_Renderer *renderer);
};
class Game {
   public:
    Game() = default;
    ~Game() = default;

    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    bool RunApplication();

   private:
    Resources res_;
    bool LoadAssets();
    void HandleEvent(const SDL_Event &event);
    void Update(float deltaTime);

    void DestroyAssets();
    void UpdateAnimation(float deltaTime);
    void Render(SDL_Renderer *renderer, Resources &res);
    void RenderZombie(SDL_Renderer *renderer, Resources &res);

    entt::registry regsitry_;
    float kFrameDuration = 0.12f;
    int kIdleFrameCount = 6;
    int kWalkFrameCount = 8;

    float zombieAnimationTime = 0.0f;
    int zombieAnimationFrame = 0;
    ZombieState zombieState = ZombieState::Idle;
};
