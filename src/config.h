#pragma once

#include <SDL3/SDL.h>

struct ApplicationConfig {
    int windowWidth = 1280;
    int windowHeight = 720;
};

class Game {
   public:
    explicit Game(SDL_Renderer* renderer);
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void HandleEvent(const SDL_Event& event);
    void Update(float deltaTime);
    void Render() const;

   private:
    bool LoadAssets();
    void DestroyAssets();

    void UpdateAnimation(float deltaTime);
}
