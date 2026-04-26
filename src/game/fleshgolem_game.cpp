#include "fleshgolem_game.h"

#include <vector>

using namespace fleshgolem_internal;

namespace {

bool LoadTextureAsset(SDL_Renderer* renderer, const std::string& fileName, const bool colorKey,
                      SDL_Texture** textureTarget, std::string* pathTarget) {
    constexpr std::array<const char*, 2> kAssetRoots{"assets/", "../assets/"};

    for (const char* assetRoot : kAssetRoots) {
        const std::string candidatePath = std::string(assetRoot) + fileName;
        SDL_Surface* surface = SDL_LoadBMP(candidatePath.c_str());
        if (surface == nullptr) {
            continue;
        }

        if (colorKey) {
            const Uint32 key = SDL_MapSurfaceRGB(surface, 255, 0, 255);
            SDL_SetSurfaceColorKey(surface, true, key);
        }

        *textureTarget = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        if (*textureTarget != nullptr) {
            SDL_SetTextureScaleMode(*textureTarget, SDL_SCALEMODE_NEAREST);
            *pathTarget = candidatePath;
            return true;
        }
    }

    pathTarget->clear();
    return false;
}

float Oscillator(const float frequency, const float timeSeconds) {
    return std::sin(2.0F * kPi * frequency * timeSeconds);
}

}  // namespace

FleshgolemGame::FleshgolemGame(SDL_Renderer* renderer)
    : renderer_(renderer), rng_(std::random_device{}()) {
    LoadAssets();
    InitializeAudio();
    ResetRun();
    sceneState_ = SceneState::Title;
    UpdateWindowTitle();
}

FleshgolemGame::~FleshgolemGame() {
    ShutdownAudio();
    DestroyAssets();
}

bool FleshgolemGame::LoadAssets() {
    DestroyAssets();
    spriteSheetPath_.clear();
    titleBannerPath_.clear();
    enemySpriteSheetPaths_.fill("");
    zoneBackdropPaths_.fill("");

    const bool playerLoaded = LoadTextureAsset(renderer_, "player_walk.bmp", true,
                                               &playerSpriteSheet_, &spriteSheetPath_);
    if (!playerLoaded) {
        SDL_Log("Failed to load player sprite sheet: %s", SDL_GetError());
    }

    const bool titleLoaded = LoadTextureAsset(renderer_, "title_banner.bmp", false,
                                              &titleBannerTexture_, &titleBannerPath_);
    if (!titleLoaded) {
        SDL_Log("Failed to load title banner: %s", SDL_GetError());
    }

    bool allEnemySpritesLoaded = true;
    for (std::size_t index = 0; index < kEnemyTypeCount; ++index) {
        const EnemyKind kind = static_cast<EnemyKind>(index);
        if (!LoadTextureAsset(renderer_, EnemySpriteFileName(kind), true,
                              &enemySpriteSheets_[index], &enemySpriteSheetPaths_[index])) {
            SDL_Log("Failed to load enemy sprite sheet for %s: %s", EnemyTemplateFor(kind).name,
                    SDL_GetError());
            allEnemySpritesLoaded = false;
        }
    }

    bool allBackdropsLoaded = true;
    for (std::size_t index = 0; index < kZoneCount; ++index) {
        if (!LoadTextureAsset(renderer_, ZoneBackdropFileName(static_cast<int>(index)), false,
                              &zoneBackdropTextures_[index], &zoneBackdropPaths_[index])) {
            SDL_Log("Failed to load backdrop %zu: %s", index, SDL_GetError());
            allBackdropsLoaded = false;
        }
    }

    return playerLoaded && titleLoaded && allEnemySpritesLoaded && allBackdropsLoaded;
}

void FleshgolemGame::DestroyAssets() {
    if (playerSpriteSheet_ != nullptr) {
        SDL_DestroyTexture(playerSpriteSheet_);
        playerSpriteSheet_ = nullptr;
    }
    if (titleBannerTexture_ != nullptr) {
        SDL_DestroyTexture(titleBannerTexture_);
        titleBannerTexture_ = nullptr;
    }

    for (SDL_Texture*& texture : enemySpriteSheets_) {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    for (SDL_Texture*& texture : zoneBackdropTextures_) {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }

    spriteSheetPath_.clear();
    titleBannerPath_.clear();
    enemySpriteSheetPaths_.fill("");
    zoneBackdropPaths_.fill("");
}

bool FleshgolemGame::InitializeAudio() {
    ShutdownAudio();

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        return false;
    }

    const SDL_AudioSpec spec{SDL_AUDIO_F32, 2, kAudioSampleRate};
    audioStream_ =
        SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (audioStream_ == nullptr) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    if (!SDL_ResumeAudioStreamDevice(audioStream_)) {
        SDL_DestroyAudioStream(audioStream_);
        audioStream_ = nullptr;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    return true;
}

void FleshgolemGame::ShutdownAudio() {
    if (audioStream_ != nullptr) {
        SDL_DestroyAudioStream(audioStream_);
        audioStream_ = nullptr;
    }

    if (SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}

void FleshgolemGame::QueueAudio(const float deltaSeconds) {
    if (audioStream_ == nullptr) {
        return;
    }

    const int queuedBytes = SDL_GetAudioStreamQueued(audioStream_);
    const int targetBytes = static_cast<int>(sizeof(float) * 2 * 2048);
    if (queuedBytes >= targetBytes * 2) {
        return;
    }

    constexpr int kFrames = 2048;
    std::vector<float> samples(static_cast<std::size_t>(kFrames) * 2U, 0.0F);

    for (int frame = 0; frame < kFrames; ++frame) {
        const float time =
            audioTime_ + static_cast<float>(frame) / static_cast<float>(kAudioSampleRate);

        float low = 0.0F;
        float mid = 0.0F;
        float high = 0.0F;

        if (sceneState_ == SceneState::Title) {
            low += 0.08F * Oscillator(72.0F, time);
            mid += 0.04F * Oscillator(144.0F + 14.0F * std::sin(time * 0.45F), time);
            high += 0.03F * Oscillator(216.0F, time * 0.5F);
        } else if (sceneState_ == SceneState::SkillTree) {
            low += 0.06F * Oscillator(96.0F, time);
            mid += 0.05F * Oscillator(192.0F + 24.0F * std::sin(time * 1.1F), time);
            high += 0.03F * Oscillator(288.0F, time);
        } else if (sceneState_ == SceneState::Victory) {
            low += 0.05F * Oscillator(110.0F, time);
            mid += 0.05F * Oscillator(220.0F, time);
            high += 0.04F * Oscillator(330.0F + 30.0F * std::sin(time * 0.8F), time);
        } else if (sceneState_ == SceneState::Defeat) {
            low += 0.07F * Oscillator(58.0F, time);
            mid += 0.03F * Oscillator(92.0F, time);
        } else {
            const RunState& runState = registry_.get<RunState>(player_);
            const float base = 78.0F + static_cast<float>(runState.zoneIndex) * 20.0F;
            low += 0.06F * Oscillator(base, time);
            mid += 0.035F * Oscillator(base * 1.5F + 8.0F * std::sin(time * 0.5F), time);
            if (combatJoined_) {
                const float beat = std::max(0.0F, std::sin(time * 7.5F));
                high += beat * 0.05F * Oscillator(base * 3.0F, time);
            }
        }

        if (attackSoundTime_ > 0.0F) {
            const float envelope = attackSoundTime_;
            high += 0.18F * envelope * Oscillator(780.0F + 120.0F * envelope, time);
        }
        if (rewardSoundTime_ > 0.0F) {
            const float envelope = rewardSoundTime_;
            mid += 0.22F * envelope * Oscillator(320.0F + 220.0F * (1.0F - envelope), time);
        }
        if (failureSoundTime_ > 0.0F) {
            const float envelope = failureSoundTime_;
            low += 0.22F * envelope * Oscillator(160.0F - 80.0F * (1.0F - envelope), time);
        }
        if (victorySoundTime_ > 0.0F) {
            const float envelope = victorySoundTime_;
            high += 0.2F * envelope * Oscillator(420.0F + 260.0F * (1.0F - envelope), time);
        }

        const float left = Clamp(low + mid + high, -0.85F, 0.85F);
        const float right =
            Clamp(low + mid * 0.9F + high * (combatJoined_ ? 0.8F : 1.0F), -0.85F, 0.85F);

        samples[static_cast<std::size_t>(frame) * 2U] = left;
        samples[static_cast<std::size_t>(frame) * 2U + 1U] = right;
    }

    SDL_PutAudioStreamData(audioStream_, samples.data(),
                           static_cast<int>(samples.size() * sizeof(float)));

    const float advanced = static_cast<float>(kFrames) / static_cast<float>(kAudioSampleRate);
    audioTime_ += advanced;
    attackSoundTime_ = std::max(0.0F, attackSoundTime_ - advanced * 3.4F);
    rewardSoundTime_ = std::max(0.0F, rewardSoundTime_ - advanced * 1.4F);
    failureSoundTime_ = std::max(0.0F, failureSoundTime_ - advanced * 0.7F);
    victorySoundTime_ = std::max(0.0F, victorySoundTime_ - advanced * 0.6F);

    (void)deltaSeconds;
}

void FleshgolemGame::TriggerAttackAudio() { attackSoundTime_ = 1.0F; }

void FleshgolemGame::TriggerRewardAudio() { rewardSoundTime_ = 1.0F; }

void FleshgolemGame::TriggerFailureAudio() { failureSoundTime_ = 1.0F; }

void FleshgolemGame::TriggerVictoryAudio() { victorySoundTime_ = 1.0F; }

void FleshgolemGame::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        const float mouseX = event.button.x;
        const float mouseY = event.button.y;

        if (sceneState_ == SceneState::Title) {
            if (PointInRect(TitleStartButtonRect(), mouseX, mouseY)) {
                ResetRun();
            }
            return;
        }

        if (paused_ && sceneState_ == SceneState::Running) {
            if (PointInRect(ResumeButtonRect(), mouseX, mouseY)) {
                paused_ = false;
                AddLog("Systems resumed.");
            } else if (PointInRect(RestartButtonRect(), mouseX, mouseY)) {
                AddLog("The homunculus is restitched from scraps.");
                ResetRun();
            }
            return;
        }

        if (sceneState_ == SceneState::SkillTree) {
            for (int index = 0; index < static_cast<int>(kSkillNodeCount); ++index) {
                if (PointInRect(SkillNodeRect(index), mouseX, mouseY)) {
                    selectedSkillIndex_ = index;
                    return;
                }
            }

            if (PointInRect(SkillSpendButtonRect(), mouseX, mouseY)) {
                TrySpendSkillPoint(selectedSkillIndex_);
            } else if (PointInRect(SkillCloseButtonRect(), mouseX, mouseY)) {
                CloseSkillTree();
            }
            return;
        }

        if (sceneState_ == SceneState::Victory || sceneState_ == SceneState::Defeat) {
            if (PointInRect(RunEndPrimaryButtonRect(), mouseX, mouseY)) {
                ResetRun();
            } else if (PointInRect(RunEndSecondaryButtonRect(), mouseX, mouseY)) {
                ReturnToTitle();
            }
            return;
        }
    }

    if (event.type != SDL_EVENT_KEY_DOWN || event.key.repeat) {
        return;
    }

    if (sceneState_ == SceneState::Title) {
        if (event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE) {
            ResetRun();
        }
        return;
    }

    if (sceneState_ == SceneState::SkillTree) {
        if (event.key.key == SDLK_LEFT) {
            selectedSkillIndex_ = FindSkillSelection(-1, 0);
        } else if (event.key.key == SDLK_RIGHT) {
            selectedSkillIndex_ = FindSkillSelection(1, 0);
        } else if (event.key.key == SDLK_UP) {
            selectedSkillIndex_ = FindSkillSelection(0, -1);
        } else if (event.key.key == SDLK_DOWN) {
            selectedSkillIndex_ = FindSkillSelection(0, 1);
        } else if (event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE) {
            TrySpendSkillPoint(selectedSkillIndex_);
        } else if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_K ||
                   event.key.key == SDLK_TAB) {
            CloseSkillTree();
        }
        return;
    }

    if (sceneState_ == SceneState::Victory || sceneState_ == SceneState::Defeat) {
        if (event.key.key == SDLK_RETURN || event.key.key == SDLK_R) {
            ResetRun();
        } else if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_T) {
            ReturnToTitle();
        } else if (sceneState_ == SceneState::Defeat && event.key.key == SDLK_E) {
            DissolveRun();
        }
        return;
    }

    if (paused_) {
        if (event.key.key == SDLK_SPACE || event.key.key == SDLK_ESCAPE) {
            paused_ = false;
            AddLog("Systems resumed.");
        }
        return;
    }

    switch (event.key.key) {
        case SDLK_SPACE:
            paused_ = true;
            AddLog("Systems paused.");
            break;
        case SDLK_R:
            AddLog("The homunculus is restitched from scraps.");
            ResetRun();
            break;
        case SDLK_E:
            DissolveRun();
            break;
        case SDLK_K:
        case SDLK_TAB:
            OpenSkillTree();
            break;
        case SDLK_N:
            if (currentEnemy_ == entt::null && currentCorpse_ == entt::null) {
                enemySpawnDelay_ = 0.0F;
                AddLog("Scouting accelerates the next encounter.");
            }
            break;
        default:
            break;
    }
}

void FleshgolemGame::Update(const float deltaSeconds) {
    const float clampedDelta = Clamp(deltaSeconds, 0.0F, kMaxDeltaSeconds);

    if (sceneState_ == SceneState::Running) {
        RecalculatePlayerStats();
        if (!paused_) {
            UpdateAnimation(clampedDelta);
            UpdateDecay(clampedDelta);
            UpdateHarvesting(clampedDelta);
            SpawnEnemyIfNeeded(clampedDelta);
            UpdateLane(clampedDelta);
            UpdateCombat(clampedDelta);
            RecalculatePlayerStats();
        }
    } else {
        UpdateAnimation(clampedDelta * 0.6F);
    }

    QueueAudio(clampedDelta);
    UpdateWindowTitle();
}

void FleshgolemGame::Render() const {
    RenderBackdrop();

    if (sceneState_ == SceneState::Title) {
        RenderTitleScreen();
        return;
    }

    RenderResourcePanel(SDL_FRect{24.0F, 20.0F, 1232.0F, 78.0F});
    RenderPlayerPanel(SDL_FRect{24.0F, 118.0F, 470.0F, 578.0F});
    RenderEncounterPanel(SDL_FRect{520.0F, 118.0F, 736.0F, 340.0F});
    RenderEventLog(SDL_FRect{520.0F, 480.0F, 736.0F, 216.0F});

    if (paused_ && sceneState_ == SceneState::Running) {
        RenderPauseOverlay();
    }
    if (sceneState_ == SceneState::SkillTree) {
        RenderSkillTreeOverlay();
    }
    if (sceneState_ == SceneState::Victory || sceneState_ == SceneState::Defeat) {
        RenderRunEndOverlay();
    }
}

const std::string& FleshgolemGame::WindowTitle() const { return windowTitle_; }

void FleshgolemGame::ResetRun() {
    registry_.clear();
    player_ = entt::null;
    currentEnemy_ = entt::null;
    currentCorpse_ = entt::null;
    runUpgrades_ = RunUpgrades{};

    enemySpawnDelay_ = 1.0F;
    enemyLaneX_ = kLaneEnemySpawnX;
    enemyApproachSpeed_ = 0.0F;
    corpseLaneX_ = kLaneEnemyContactX;
    playerAnimationTime_ = 0.0F;
    playerAnimationFrame_ = 0;
    selectedSkillIndex_ = 0;
    combatJoined_ = false;
    paused_ = false;
    sceneState_ = SceneState::Running;
    endTitle_.clear();
    endSubtitle_.clear();
    eventLog_.clear();

    SpawnPlayer();
    BuildStarterBody();
    RebuildRunUpgradesFromSkills();
    RecalculatePlayerStats();

    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);
    AddLog("The Fleshgolem demo begins.");
    AddLog(std::string("Entering ") + zone.name + ": " + zone.subtitle);
    if (!titleBannerPath_.empty()) {
        AddLog("Loaded title banner: " + titleBannerPath_);
    }
    if (!spriteSheetPath_.empty()) {
        AddLog("Loaded walk sprite sheet: " + spriteSheetPath_);
    }
}

void FleshgolemGame::ReturnToTitle() {
    ResetRun();
    sceneState_ = SceneState::Title;
    AddLog("Returned to the stitching table.");
}

void FleshgolemGame::AddLog(const std::string& message) {
    eventLog_.push_front(message);
    while (eventLog_.size() > 11) {
        eventLog_.pop_back();
    }
}

void FleshgolemGame::UpdateWindowTitle() {
    if (sceneState_ == SceneState::Title) {
        windowTitle_ = "idlegolem | demo | press Enter to begin";
        return;
    }

    if (player_ == entt::null || !registry_.valid(player_)) {
        windowTitle_ = "idlegolem | demo";
        return;
    }

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);

    std::ostringstream title;
    title << "idlegolem | demo | " << zone.name << " | wave " << (runState.zoneEncounter + 1) << "/"
          << runState.encountersPerZone << " | kills " << resources.kills << " | biomass "
          << resources.biomass << " | research " << resources.research << " | bank "
          << bankedEssence_;

    if (sceneState_ == SceneState::SkillTree) {
        title << " | skill tree";
    } else if (sceneState_ == SceneState::Victory) {
        title << " | demo complete";
    } else if (sceneState_ == SceneState::Defeat) {
        title << " | run failed";
    } else if (paused_) {
        title << " | paused";
    }

    windowTitle_ = title.str();
}
