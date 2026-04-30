#include <SDL3/SDL.h>

#include <algorithm>
#include <vector>

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

int SpawnZombie() { return 0; }

struct Resources {
    std::vector<SDL_Texture *> textures;
    SDL_Texture *zombieIdle, *zombieWalk;

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

    void LoadAssets(SDL_Renderer *renderer) {
        DestroyAssets();
        zombieIdle = loadTexture(renderer,
                                 "assets/pppack/Enemies/Zombie_Axe/"
                                 "Zombie_Axe_Side-left_Idle-Sheet6.png");
        textures.push_back(zombieIdle);
        zombieWalk = loadTexture(renderer,
                                 "assets/pppack/Enemies/Zombie_Axe/"
                                 "Zombie_Axe_Side-left_Walk-Sheet8.png");
        textures.push_back(zombieWalk);
    }
};

void HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
        const float mouseX = event.button.x;
        const float mouseY = event.button.y;
        SDL_Log("Mouse X: %f | Mouse Y: %f", mouseX, mouseY);
        SpawnZombie();
    }
}

void Update(float deltaTime) {
    const float clampedDelta = std::clamp(deltaTime, 0.0F, 0.05F);
};

enum ZombieState { Idle, Walk, Dig };

void RenderZombie(SDL_Renderer *renderer, Resources &res) {
    SDL_FRect src{.x = 0.0f, .y = 0.0f, .w = 22.0f, .h = 21.0f};
    SDL_FRect dst{.x = 100.0f, .y = 100.0f, .w = 128.0f, .h = 128.0f};

    ZombieState zState = ZombieState::Idle;
    switch (zState) {
        case ZombieState::Idle:
            SDL_RenderTexture(renderer, res.zombieIdle, &src, &dst);
            break;
        case ZombieState::Walk:
            SDL_RenderTexture(renderer, res.zombieWalk, &src, &dst);
            break;
        case ZombieState::Dig:
            break;
    }
}

void Render(SDL_Renderer *renderer, Resources &res) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_FRect box{.x = 100.0F, .y = 100.0F, .w = 200.0F, .h = 200.0F};
    SDL_RenderFillRect(renderer, &box);

    RenderZombie(renderer, res);
}

bool RunApplication() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    SDL_Window *window =
        SDL_CreateWindow("Game Window", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == nullptr) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    Resources res;
    res.LoadAssets(renderer);

    bool running = true;
    Uint64 lastTicks = SDL_GetTicks();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else {
                HandleEvent(event);
            }
        }
        const Uint64 currentTicks = SDL_GetTicks();
        const float deltaTime =
            static_cast<float>(currentTicks - lastTicks) / 1000.0F;
        lastTicks = currentTicks;

        Update(deltaTime);

        Render(renderer, res);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}

int main(int argc, char *argv[]) {
    if (!RunApplication()) {
        SDL_Log("Application failed to run.");
        return 1;
    }
}
