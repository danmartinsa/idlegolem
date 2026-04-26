#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <deque>
#include <entt/entt.hpp>
#include <random>
#include <string>

#include "engine/game.h"
#include "game/fleshgolem_internal.h"

class FleshgolemGame : public engine::Game {
   public:
    explicit FleshgolemGame(SDL_Renderer* renderer);
    ~FleshgolemGame();

    FleshgolemGame(const FleshgolemGame&) = delete;
    FleshgolemGame& operator=(const FleshgolemGame&) = delete;

    void HandleEvent(const SDL_Event& event) override;
    void Update(float deltaSeconds) override;
    void Render() const override;

    [[nodiscard]] const std::string& WindowTitle() const override;

   private:
    bool LoadAssets();
    void DestroyAssets();
    bool InitializeAudio();
    void ShutdownAudio();
    void QueueAudio(float deltaSeconds);
    void TriggerAttackAudio();
    void TriggerRewardAudio();
    void TriggerFailureAudio();
    void TriggerVictoryAudio();

    void ResetRun();
    void AdvanceToNextZone();
    void OpenSkillTree();
    void CloseSkillTree();
    void AwardExperience(int amount);
    bool TrySpendSkillPoint(int skillIndex);
    void RebuildRunUpgradesFromSkills();
    [[nodiscard]] int FindFirstSpendableSkill() const;
    [[nodiscard]] int FindSkillSelection(int directionX, int directionY) const;
    void FinishRun(bool victory);
    void ReturnToTitle();

    void SpawnPlayer();
    void BuildStarterBody();
    void SpawnEnemyIfNeeded(float deltaSeconds);
    void SpawnEnemy(int tier, bool elite, bool boss);

    void UpdateAnimation(float deltaSeconds);
    void UpdateLane(float deltaSeconds);
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
    void RenderTitleScreen() const;
    void RenderResourcePanel(const SDL_FRect& panel) const;
    void RenderPlayerPanel(const SDL_FRect& panel) const;
    void RenderEncounterPanel(const SDL_FRect& panel) const;
    void RenderLane(const SDL_FRect& laneRect) const;
    void RenderEventLog(const SDL_FRect& panel) const;
    void RenderPauseOverlay() const;
    void RenderSkillTreeOverlay() const;
    void RenderRunEndOverlay() const;

    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* playerSpriteSheet_ = nullptr;
    SDL_Texture* titleBannerTexture_ = nullptr;
    std::array<SDL_Texture*, fleshgolem_internal::kEnemyTypeCount>
        enemySpriteSheets_{};
    std::array<SDL_Texture*, fleshgolem_internal::kZoneCount>
        zoneBackdropTextures_{};
    SDL_AudioStream* audioStream_ = nullptr;

    entt::registry registry_;
    entt::entity player_ = entt::null;
    entt::entity currentEnemy_ = entt::null;
    entt::entity currentCorpse_ = entt::null;

    float enemySpawnDelay_ = 0.0F;
    float enemyLaneX_ = 0.0F;
    float enemyApproachSpeed_ = 0.0F;
    float corpseLaneX_ = 0.0F;
    float playerAnimationTime_ = 0.0F;
    float audioTime_ = 0.0F;
    float attackSoundTime_ = 0.0F;
    float rewardSoundTime_ = 0.0F;
    float failureSoundTime_ = 0.0F;
    float victorySoundTime_ = 0.0F;

    int playerAnimationFrame_ = 0;
    int bankedEssence_ = 0;
    int completedRuns_ = 0;
    int lastPayout_ = 0;
    int selectedSkillIndex_ = 0;

    bool combatJoined_ = false;
    bool paused_ = false;

    fleshgolem_internal::SceneState sceneState_ =
        fleshgolem_internal::SceneState::Title;
    fleshgolem_internal::RunUpgrades runUpgrades_{};

    std::mt19937 rng_;
    std::deque<std::string> eventLog_;
    std::string windowTitle_;
    std::string spriteSheetPath_;
    std::string titleBannerPath_;
    std::array<std::string, fleshgolem_internal::kEnemyTypeCount>
        enemySpriteSheetPaths_{};
    std::array<std::string, fleshgolem_internal::kZoneCount>
        zoneBackdropPaths_{};
    std::string endTitle_;
    std::string endSubtitle_;
};
