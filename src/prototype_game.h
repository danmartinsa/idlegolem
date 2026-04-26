#pragma once

#include <SDL3/SDL.h>

#include <deque>
#include <entt/entt.hpp>
#include <random>
#include <string>

class PrototypeGame {
   public:
    explicit PrototypeGame(SDL_Renderer* renderer);

    void HandleEvent(const SDL_Event& event);
    void Update(float deltaSeconds);
    void Render() const;

    [[nodiscard]] const std::string& WindowTitle() const;

   private:
    void ResetRun();
    void SpawnPlayer();
    void BuildStarterBody();
    void SpawnEnemyIfNeeded(float deltaSeconds);
    void SpawnEnemy(int tier, bool elite);
    void RecalculatePlayerStats();
    void UpdateCombat(float deltaSeconds);
    void ResolveAttack(entt::entity attacker, entt::entity defender);
    void OnEnemyDefeated(entt::entity enemy);
    void UpdateHarvesting(float deltaSeconds);
    void HarvestCorpse(entt::entity corpse);
    void UpdateDecay(float deltaSeconds);
    void DissolveRun();
    void AddLog(const std::string& message);
    void UpdateWindowTitle();

    void RenderBackdrop() const;
    void RenderResourcePanel(const SDL_FRect& panel) const;
    void RenderPlayerPanel(const SDL_FRect& panel) const;
    void RenderEncounterPanel(const SDL_FRect& panel) const;
    void RenderEventLog(const SDL_FRect& panel) const;

    SDL_Renderer* renderer_ = nullptr;
    entt::registry registry_;
    entt::entity player_ = entt::null;
    entt::entity currentEnemy_ = entt::null;
    entt::entity currentCorpse_ = entt::null;
    float enemySpawnDelay_ = 0.0f;
    bool paused_ = false;
    bool runFailed_ = false;
    int bankedEssence_ = 0;
    int completedRuns_ = 0;
    std::mt19937 rng_;
    std::deque<std::string> eventLog_;
    std::string windowTitle_;
};
