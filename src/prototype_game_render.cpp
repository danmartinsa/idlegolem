#include <sstream>

#include "prototype_game.h"

using namespace prototype_game_internal;

void PrototypeGame::RenderBackdrop() const {
    const SDL_FRect fullScreen{0.0F, 0.0F, kWindowWidth, kWindowHeight};

    if (sceneState_ == SceneState::Title) {
        FillRect(renderer_, fullScreen, SDL_Color{16, 10, 16, 255});
        if (titleBannerTexture_ != nullptr) {
            SDL_RenderTexture(renderer_, titleBannerTexture_, nullptr, &fullScreen);
        }
        FillRect(renderer_, fullScreen, SDL_Color{8, 6, 10, 120});
        return;
    }

    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);

    FillRect(renderer_, fullScreen, zone.skyTop);
    if (zoneBackdropTextures_[static_cast<std::size_t>(runState.zoneIndex)] != nullptr) {
        SDL_RenderTexture(renderer_,
                          zoneBackdropTextures_[static_cast<std::size_t>(runState.zoneIndex)],
                          nullptr, &fullScreen);
    }
    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, kWindowWidth, 220.0F}, zone.skyBottom);
    FillRect(renderer_, fullScreen, SDL_Color{0, 0, 0, 44});
}

void PrototypeGame::RenderTitleScreen() const {
    const SDL_FRect panel{232.0F, 122.0F, 816.0F, 456.0F};
    const SDL_FRect startButton = TitleStartButtonRect();

    FillRect(renderer_, panel, SDL_Color{24, 18, 24, 228});
    DrawRect(renderer_, panel, SDL_Color{206, 188, 174, 255});

    DrawText(renderer_, 332.0F, 150.0F, SDL_Color{242, 226, 214, 255}, "FLESHGOLEM");
    DrawText(renderer_, 350.0F, 176.0F, SDL_Color{206, 188, 176, 255},
             "Gameplay-first demo built from the original prototype.");

    DrawText(renderer_, 282.0F, 226.0F, SDL_Color{220, 208, 198, 255}, "What this demo adds:");
    DrawText(renderer_, 300.0F, 252.0F, SDL_Color{190, 184, 178, 255},
             "- title flow, themed zones, elites, bosses, reward drafts");
    DrawText(renderer_, 300.0F, 274.0F, SDL_Color{190, 184, 178, 255},
             "- stronger run progression, upgrades, research, and payoff");
    DrawText(renderer_, 300.0F, 296.0F, SDL_Color{190, 184, 178, 255},
             "- generated backdrop assets, title art, and synthesized SDL audio");
    DrawText(renderer_, 300.0F, 318.0F, SDL_Color{190, 184, 178, 255},
             "- auto-combat, harvesting, grafting, decay, and dissolve still intact");

    std::ostringstream metaLine;
    metaLine << "Banked Essence: " << bankedEssence_ << "   Completed Runs: " << completedRuns_
             << "   Last Payout: " << lastPayout_;
    DrawText(renderer_, 300.0F, 368.0F, SDL_Color{222, 204, 166, 255}, metaLine.str());

    DrawText(renderer_, 300.0F, 402.0F, SDL_Color{188, 196, 188, 255},
             "Controls once inside the run: Space pause, R restart, E dissolve, N scout.");

    FillRect(renderer_, startButton, SDL_Color{108, 136, 104, 255});
    DrawRect(renderer_, startButton, SDL_Color{236, 232, 224, 255});
    DrawText(renderer_, startButton.x + 112.0F, startButton.y + 17.0F, SDL_Color{16, 16, 16, 255},
             "Start Demo");

    DrawText(renderer_, 424.0F, 522.0F, SDL_Color{174, 168, 160, 255},
             "Press Enter or click the button to awaken a fresh homunculus.");
}

void PrototypeGame::RenderResourcePanel(const SDL_FRect& panel) const {
    FillRect(renderer_, panel, SDL_Color{32, 24, 34, 240});
    DrawRect(renderer_, panel, SDL_Color{118, 96, 118, 255});

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);
    const StabilityState& stability = registry_.get<StabilityState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);

    DrawText(renderer_, panel.x + 16.0F, panel.y + 10.0F, SDL_Color{234, 214, 214, 255},
             "FLESHGOLEM DEMO  |  Space pause  R restart  E dissolve  N scout  1/2/3 reward picks");

    std::ostringstream resourceLine;
    resourceLine << "Zone: " << zone.name << "   Wave: " << (runState.zoneEncounter + 1) << "/"
                 << runState.encountersPerZone << "   Kills: " << resources.kills
                 << "   Biomass: " << resources.biomass << "   Bone Meal: " << resources.boneMeal
                 << "   Research: " << resources.research << "   Run Essence: " << resources.essence
                 << "   Banked Essence: " << bankedEssence_;
    DrawText(renderer_, panel.x + 16.0F, panel.y + 30.0F, SDL_Color{206, 182, 178, 255},
             resourceLine.str());

    std::ostringstream statusLine;
    statusLine << "Grafts: " << resources.grafts << "   Salvaged: " << resources.salvaged
               << "   Reward Drafts: " << runState.rewardDrafts
               << "   Bosses: " << runState.bossesDefeated << "   Stability "
               << FormatNumber(stability.demand) << "/" << FormatNumber(stability.capacity);
    if (paused_) {
        statusLine << "   [PAUSED]";
    }
    if (sceneState_ == SceneState::RewardDraft) {
        statusLine << "   [REWARD DRAFT]";
    }
    DrawText(
        renderer_, panel.x + 16.0F, panel.y + 50.0F,
        stability.overload > 0.0F ? SDL_Color{214, 108, 98, 255} : SDL_Color{154, 196, 142, 255},
        statusLine.str());
}

void PrototypeGame::RenderPlayerPanel(const SDL_FRect& panel) const {
    FillRect(renderer_, panel, SDL_Color{28, 24, 30, 240});
    DrawRect(renderer_, panel, SDL_Color{92, 82, 98, 255});

    const Name& name = registry_.get<Name>(player_);
    const Health& health = registry_.get<Health>(player_);
    const Attributes& attributes = registry_.get<Attributes>(player_);
    const CombatStats& stats = registry_.get<CombatStats>(player_);
    const StabilityState& stability = registry_.get<StabilityState>(player_);
    const Body& body = registry_.get<Body>(player_);

    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F, SDL_Color{232, 216, 216, 255},
             name.value);
    DrawBar(renderer_, SDL_FRect{panel.x + 16.0F, panel.y + 34.0F, 220.0F, 12.0F},
            health.current / std::max(1.0F, health.maximum), SDL_Color{170, 72, 76, 255},
            SDL_Color{70, 36, 44, 255});

    std::ostringstream hpLine;
    hpLine << "HP " << static_cast<int>(std::round(health.current)) << "/"
           << static_cast<int>(std::round(health.maximum));
    DrawText(renderer_, panel.x + 246.0F, panel.y + 36.0F, SDL_Color{214, 196, 192, 255},
             hpLine.str());

    std::ostringstream statLineOne;
    statLineOne << "Might " << FormatNumber(attributes.might) << "  Vitality "
                << FormatNumber(attributes.vitality) << "  Agility "
                << FormatNumber(attributes.agility);
    DrawText(renderer_, panel.x + 16.0F, panel.y + 60.0F, SDL_Color{186, 194, 182, 255},
             statLineOne.str());

    std::ostringstream statLineTwo;
    statLineTwo << "Reason " << FormatNumber(attributes.reason) << "  Instinct "
                << FormatNumber(attributes.instinct) << "  Corruption "
                << FormatNumber(attributes.corruption);
    DrawText(renderer_, panel.x + 16.0F, panel.y + 78.0F, SDL_Color{186, 194, 182, 255},
             statLineTwo.str());

    std::ostringstream combatLine;
    combatLine << "ATK " << FormatNumber(stats.attackPower) << "  SPD "
               << FormatNumber(1.0F / stats.attackInterval, 2) << "  CRIT "
               << static_cast<int>(std::round(stats.critChance * 100.0F)) << "%  ARM "
               << FormatNumber(stats.armor) << "  EVA "
               << static_cast<int>(std::round(stats.evasion * 100.0F)) << "%";
    DrawText(renderer_, panel.x + 16.0F, panel.y + 96.0F, SDL_Color{172, 186, 202, 255},
             combatLine.str());

    std::ostringstream stabilityLine;
    stabilityLine << "Stability " << FormatNumber(stability.demand) << "/"
                  << FormatNumber(stability.capacity) << "  Harvest x"
                  << FormatNumber(runUpgrades_.harvestMultiplier, 2) << "  Decay x"
                  << FormatNumber(runUpgrades_.decayMultiplier, 2);
    if (stability.overload > 0.0F) {
        stabilityLine << "  OVERLOAD +" << FormatNumber(stability.overload);
    }
    DrawText(
        renderer_, panel.x + 16.0F, panel.y + 114.0F,
        stability.overload > 0.0F ? SDL_Color{214, 108, 98, 255} : SDL_Color{144, 182, 144, 255},
        stabilityLine.str());

    const auto slotRects = BuildSlotRects(panel);
    for (std::size_t index = 0; index < kSlotCount; ++index) {
        const SlotType slot = kAllSlots[index];
        const entt::entity partEntity = body.slots[index];
        SDL_Color fill{58, 54, 64, 255};
        std::string freshnessText = "--";

        if (partEntity != entt::null && registry_.valid(partEntity)) {
            const BodyPart& part = registry_.get<BodyPart>(partEntity);
            fill = part.color;
            freshnessText = std::to_string(static_cast<int>(std::round(part.freshness)));
        }

        FillRect(renderer_, slotRects[index], fill);
        DrawRect(renderer_, slotRects[index], SDL_Color{214, 214, 214, 255});
        DrawText(renderer_, slotRects[index].x + 6.0F, slotRects[index].y + 6.0F,
                 SDL_Color{18, 18, 18, 255}, SlotShortLabel(slot));
        DrawText(renderer_, slotRects[index].x + 6.0F,
                 slotRects[index].y + slotRects[index].h - 14.0F, SDL_Color{18, 18, 18, 255},
                 freshnessText);
    }

    DrawText(renderer_, panel.x + 278.0F, panel.y + 156.0F, SDL_Color{212, 202, 196, 255},
             "Equipped anatomy");
    float textY = panel.y + 176.0F;
    for (const SlotType slot : kAllSlots) {
        const entt::entity partEntity = body.slots[SlotIndex(slot)];
        std::ostringstream line;
        line << SlotLabel(slot) << ": ";
        SDL_Color textColor{178, 168, 168, 255};
        if (partEntity != entt::null && registry_.valid(partEntity)) {
            const BodyPart& part = registry_.get<BodyPart>(partEntity);
            line << part.name << " [" << RarityName(part.rarity) << "] "
                 << static_cast<int>(std::round(part.freshness)) << "%";
            textColor = part.color;
        } else {
            line << "empty";
        }
        DrawText(renderer_, panel.x + 278.0F, textY, textColor, line.str());
        textY += 18.0F;
    }
}

void PrototypeGame::RenderEncounterPanel(const SDL_FRect& panel) const {
    FillRect(renderer_, panel, SDL_Color{30, 22, 26, 240});
    DrawRect(renderer_, panel, SDL_Color{102, 80, 90, 255});

    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);

    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F, SDL_Color{232, 216, 216, 255}, zone.name);
    DrawText(renderer_, panel.x + 168.0F, panel.y + 12.0F, SDL_Color{190, 184, 178, 255},
             zone.subtitle);

    const SDL_FRect laneRect{panel.x + 16.0F, panel.y + 38.0F, panel.w - 32.0F, 210.0F};
    RenderLane(laneRect);

    DrawBar(renderer_, SDL_FRect{panel.x + 18.0F, panel.y + 258.0F, 340.0F, 10.0F},
            static_cast<float>(runState.zoneEncounter) /
                static_cast<float>(std::max(1, runState.encountersPerZone)),
            zone.accent, SDL_Color{62, 52, 58, 255});

    if (currentEnemy_ != entt::null && registry_.valid(currentEnemy_)) {
        const Name& name = registry_.get<Name>(currentEnemy_);
        const Health& health = registry_.get<Health>(currentEnemy_);
        const Enemy& enemy = registry_.get<Enemy>(currentEnemy_);

        DrawText(renderer_, panel.x + 18.0F, panel.y + 278.0F, SDL_Color{220, 206, 194, 255},
                 name.value);
        DrawBar(renderer_, SDL_FRect{panel.x + 18.0F, panel.y + 296.0F, 260.0F, 12.0F},
                health.current / std::max(1.0F, health.maximum), SDL_Color{182, 86, 96, 255},
                SDL_Color{84, 38, 48, 255});

        std::ostringstream line;
        line << "Tier " << enemy.tier << "  "
             << (combatJoined_ ? "CONTACT - auto-combat active" : "ADVANCING FROM THE RIGHT");
        if (enemy.elite) {
            line << "  [ELITE]";
        }
        if (enemy.boss) {
            line << "  [BOSS]";
        }
        DrawText(renderer_, panel.x + 18.0F, panel.y + 314.0F, SDL_Color{196, 188, 182, 255},
                 line.str());
    } else if (currentCorpse_ != entt::null && registry_.valid(currentCorpse_)) {
        const Corpse& corpse = registry_.get<Corpse>(currentCorpse_);
        DrawText(renderer_, panel.x + 18.0F, panel.y + 278.0F, SDL_Color{208, 204, 194, 255},
                 registry_.get<Name>(currentCorpse_).value);
        DrawBar(renderer_, SDL_FRect{panel.x + 18.0F, panel.y + 296.0F, 260.0F, 12.0F},
                1.0F - corpse.harvestTimer / std::max(0.01F, corpse.maxHarvestTimer),
                SDL_Color{112, 172, 96, 255}, SDL_Color{50, 74, 46, 255});
        DrawText(renderer_, panel.x + 18.0F, panel.y + 314.0F, SDL_Color{180, 196, 162, 255},
                 corpse.boss ? "Boss corpse: harvesting unlocks a major reward draft."
                             : "Auto-harvesting the corpse for resources and salvage.");
    } else {
        DrawText(renderer_, panel.x + 18.0F, panel.y + 278.0F, SDL_Color{208, 198, 188, 255},
                 "The homunculus keeps walking while the next enemy approaches.");
        DrawText(renderer_, panel.x + 18.0F, panel.y + 300.0F, SDL_Color{176, 170, 164, 255},
                 "Every zone ends in a boss and a reward choice.");
    }
}

void PrototypeGame::RenderLane(const SDL_FRect& laneRect) const {
    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);
    const float groundY = laneRect.y + kLaneGroundOffsetY;

    FillRect(renderer_, laneRect, SDL_Color{28, 20, 26, 255});
    if (zoneBackdropTextures_[static_cast<std::size_t>(runState.zoneIndex)] != nullptr) {
        SDL_RenderTexture(renderer_,
                          zoneBackdropTextures_[static_cast<std::size_t>(runState.zoneIndex)],
                          nullptr, &laneRect);
    }
    FillRect(renderer_,
             SDL_FRect{laneRect.x, laneRect.y + 18.0F, laneRect.w, groundY - laneRect.y - 18.0F},
             zone.skyBottom);
    FillRect(renderer_,
             SDL_FRect{laneRect.x, groundY, laneRect.w, laneRect.h - (groundY - laneRect.y)},
             zone.ground);

    for (int stripe = 0; stripe < 7; ++stripe) {
        const float stripeX = laneRect.x + 18.0F + static_cast<float>(stripe) * 98.0F;
        FillRect(renderer_, SDL_FRect{stripeX, groundY + 22.0F, 44.0F, 4.0F},
                 SDL_Color{96, 70, 64, 255});
    }

    DrawRect(renderer_, laneRect, SDL_Color{136, 112, 106, 255});
    DrawRect(renderer_,
             SDL_FRect{laneRect.x + kLaneEnemyContactX - 10.0F, laneRect.y + 22.0F, 12.0F,
                       laneRect.h - 40.0F},
             zone.accent);

    const SDL_FRect playerRect{laneRect.x + kLanePlayerX, groundY - kPlayerRenderHeight,
                               kPlayerRenderWidth, kPlayerRenderHeight};
    if (playerSpriteSheet_ != nullptr) {
        const SDL_FRect sourceRect{static_cast<float>(playerAnimationFrame_ * 64), 0.0F, 64.0F,
                                   64.0F};
        SDL_RenderTexture(renderer_, playerSpriteSheet_, &sourceRect, &playerRect);
    } else {
        FillRect(renderer_, playerRect, SDL_Color{120, 72, 86, 255});
        DrawRect(renderer_, playerRect, SDL_Color{220, 216, 210, 255});
    }

    DrawText(renderer_, laneRect.x + 24.0F, laneRect.y + 10.0F, SDL_Color{210, 204, 198, 255},
             "Walk in place, absorb the zone, and survive the boss reward loop.");

    const Health& playerHealth = registry_.get<Health>(player_);
    DrawBar(renderer_,
            SDL_FRect{playerRect.x + 8.0F, playerRect.y - 18.0F, playerRect.w - 16.0F, 10.0F},
            playerHealth.current / std::max(1.0F, playerHealth.maximum),
            SDL_Color{170, 76, 78, 255}, SDL_Color{76, 34, 40, 255});

    if (currentEnemy_ != entt::null && registry_.valid(currentEnemy_)) {
        const Enemy& enemy = registry_.get<Enemy>(currentEnemy_);
        const EnemyTemplate enemyTemplate = EnemyTemplateFor(enemy.kind);
        const Health& enemyHealth = registry_.get<Health>(currentEnemy_);

        const SDL_FRect enemyRect{laneRect.x + enemyLaneX_, groundY - kEnemyRenderHeight,
                                  kEnemyRenderWidth, kEnemyRenderHeight};
        SDL_Texture* enemySprite = enemySpriteSheets_[EnemyKindIndex(enemy.kind)];
        if (enemySprite != nullptr) {
            const SDL_FRect sourceRect{static_cast<float>(playerAnimationFrame_ * 64), 0.0F, 64.0F,
                                       64.0F};
            SDL_RenderTexture(renderer_, enemySprite, &sourceRect, &enemyRect);
            if (enemy.elite || enemy.boss) {
                DrawRect(renderer_,
                         SDL_FRect{enemyRect.x + 12.0F, enemyRect.y + 8.0F, enemyRect.w - 24.0F,
                                   enemyRect.h - 16.0F},
                         enemy.boss ? SDL_Color{214, 220, 126, 255} : SDL_Color{226, 194, 88, 255});
            }
        } else {
            RenderEnemyFigure(renderer_, enemyRect, enemyTemplate.color, enemy.elite || enemy.boss);
        }
        DrawBar(renderer_,
                SDL_FRect{enemyRect.x + 8.0F, enemyRect.y - 18.0F, enemyRect.w - 16.0F, 10.0F},
                enemyHealth.current / std::max(1.0F, enemyHealth.maximum),
                SDL_Color{184, 94, 98, 255}, SDL_Color{82, 38, 44, 255});

        DrawText(renderer_, enemyRect.x + 18.0F, enemyRect.y + enemyRect.h + 8.0F,
                 SDL_Color{220, 214, 204, 255},
                 enemy.boss ? "BOSS" : (combatJoined_ ? "ENGAGED" : "APPROACHING"));
    } else if (currentCorpse_ != entt::null && registry_.valid(currentCorpse_)) {
        const Corpse& corpse = registry_.get<Corpse>(currentCorpse_);
        const EnemyTemplate enemyTemplate = EnemyTemplateFor(corpse.source);
        const SDL_FRect corpseRect{laneRect.x + corpseLaneX_, groundY - 48.0F, kEnemyRenderWidth,
                                   52.0F};
        RenderCorpsePile(renderer_, corpseRect, enemyTemplate.color);
        DrawText(renderer_, corpseRect.x + 24.0F, corpseRect.y - 18.0F,
                 SDL_Color{198, 212, 188, 255}, corpse.boss ? "HARVEST BOSS" : "HARVEST");
    } else {
        DrawText(renderer_, laneRect.x + kLaneEnemySpawnX - 66.0F, laneRect.y + 34.0F,
                 SDL_Color{182, 182, 192, 255}, "ENTRY");
    }
}

void PrototypeGame::RenderEventLog(const SDL_FRect& panel) const {
    FillRect(renderer_, panel, SDL_Color{24, 22, 24, 240});
    DrawRect(renderer_, panel, SDL_Color{90, 82, 88, 255});
    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F, SDL_Color{224, 214, 206, 255},
             "Recent events");

    float y = panel.y + 34.0F;
    int lineIndex = 0;
    for (const std::string& line : eventLog_) {
        const SDL_Color color =
            (lineIndex == 0) ? SDL_Color{220, 210, 198, 255} : SDL_Color{166, 166, 166, 255};
        DrawText(renderer_, panel.x + 16.0F, y, color, line);
        y += 18.0F;
        ++lineIndex;
    }
}

void PrototypeGame::RenderPauseOverlay() const {
    const SDL_FRect modal = PauseModalRect();
    const SDL_FRect resumeButton = ResumeButtonRect();
    const SDL_FRect restartButton = RestartButtonRect();

    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, kWindowWidth, kWindowHeight},
             SDL_Color{0, 0, 0, 150});
    FillRect(renderer_, modal, SDL_Color{40, 28, 36, 245});
    DrawRect(renderer_, modal, SDL_Color{188, 172, 168, 255});

    DrawText(renderer_, modal.x + 148.0F, modal.y + 32.0F, SDL_Color{238, 226, 220, 255}, "PAUSED");
    DrawText(renderer_, modal.x + 54.0F, modal.y + 74.0F, SDL_Color{208, 198, 192, 255},
             "The lane, combat, harvesting, and decay are frozen.");
    DrawText(renderer_, modal.x + 74.0F, modal.y + 94.0F, SDL_Color{176, 168, 162, 255},
             "Press Space or click a button to continue.");

    FillRect(renderer_, resumeButton, SDL_Color{92, 132, 108, 255});
    DrawRect(renderer_, resumeButton, SDL_Color{228, 228, 220, 255});
    DrawText(renderer_, resumeButton.x + 96.0F, resumeButton.y + 13.0F, SDL_Color{18, 18, 18, 255},
             "Resume");

    FillRect(renderer_, restartButton, SDL_Color{142, 84, 90, 255});
    DrawRect(renderer_, restartButton, SDL_Color{228, 228, 220, 255});
    DrawText(renderer_, restartButton.x + 94.0F, restartButton.y + 13.0F,
             SDL_Color{18, 18, 18, 255}, "Restart");
}

void PrototypeGame::RenderRewardOverlay() const {
    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, kWindowWidth, kWindowHeight},
             SDL_Color{0, 0, 0, 168});

    DrawText(renderer_, 444.0F, 132.0F, SDL_Color{242, 232, 224, 255}, rewardDraftTitle_);
    DrawText(renderer_, 308.0F, 156.0F, SDL_Color{204, 196, 188, 255}, rewardDraftSubtitle_);

    for (int index = 0; index < static_cast<int>(kRewardChoiceCount); ++index) {
        const RewardOption& option = rewardOptions_[static_cast<std::size_t>(index)];
        const SDL_FRect card = RewardCardRect(index);
        const SDL_Color border =
            (index == rewardSelectionIndex_) ? SDL_Color{240, 236, 224, 255} : option.color;

        FillRect(renderer_, card, SDL_Color{24, 22, 28, 236});
        FillRect(renderer_, SDL_FRect{card.x + 8.0F, card.y + 8.0F, card.w - 16.0F, 28.0F},
                 option.color);
        DrawRect(renderer_, card, border);

        std::ostringstream label;
        label << "[" << (index + 1) << "] " << option.name;
        DrawText(renderer_, card.x + 16.0F, card.y + 16.0F, SDL_Color{18, 18, 18, 255},
                 label.str());
        DrawText(renderer_, card.x + 16.0F, card.y + 56.0F, SDL_Color{224, 214, 206, 255},
                 option.description);

        std::ostringstream statLine;
        statLine << "ATK+" << FormatNumber(option.attackBonus) << "  HP+"
                 << FormatNumber(option.healthBonus) << "  ARM+" << FormatNumber(option.armorBonus);
        DrawText(renderer_, card.x + 16.0F, card.y + 98.0F, SDL_Color{176, 200, 214, 255},
                 statLine.str());

        std::ostringstream attrLine;
        attrLine << "M " << FormatNumber(option.attributeBonus.might) << "  V "
                 << FormatNumber(option.attributeBonus.vitality) << "  A "
                 << FormatNumber(option.attributeBonus.agility) << "  R "
                 << FormatNumber(option.attributeBonus.reason) << "  I "
                 << FormatNumber(option.attributeBonus.instinct);
        DrawText(renderer_, card.x + 16.0F, card.y + 122.0F, SDL_Color{188, 188, 182, 255},
                 attrLine.str());

        std::ostringstream utilityLine;
        utilityLine << "Harvest x" << FormatNumber(option.harvestMultiplierFactor, 2) << "  Decay x"
                    << FormatNumber(option.decayMultiplierFactor, 2) << "  Heal "
                    << static_cast<int>(std::round(option.immediateHealPercent * 100.0F)) << "%";
        DrawText(renderer_, card.x + 16.0F, card.y + 146.0F, SDL_Color{172, 192, 164, 255},
                 utilityLine.str());

        std::ostringstream resourcesLine;
        resourcesLine << "Biomass +" << option.biomassBonus << "  Bone +" << option.boneMealBonus
                      << "  Essence +" << option.essenceBonus << "  Research +"
                      << option.researchBonus;
        DrawText(renderer_, card.x + 16.0F, card.y + 170.0F, SDL_Color{204, 188, 150, 255},
                 resourcesLine.str());
    }

    DrawText(renderer_, 356.0F, 500.0F, SDL_Color{204, 198, 190, 255},
             "Choose with 1/2/3, Left/Right + Enter, or click a card.");
}

void PrototypeGame::RenderRunEndOverlay() const {
    const SDL_FRect modal{382.0F, 182.0F, 516.0F, 386.0F};
    const SDL_FRect primaryButton = RunEndPrimaryButtonRect();
    const SDL_FRect secondaryButton = RunEndSecondaryButtonRect();

    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, kWindowWidth, kWindowHeight},
             SDL_Color{0, 0, 0, 176});
    FillRect(renderer_, modal, SDL_Color{34, 26, 34, 245});
    DrawRect(renderer_, modal, SDL_Color{206, 192, 180, 255});

    DrawText(renderer_, modal.x + 158.0F, modal.y + 30.0F, SDL_Color{242, 232, 224, 255},
             endTitle_);
    DrawText(renderer_, modal.x + 48.0F, modal.y + 70.0F, SDL_Color{212, 204, 196, 255},
             endSubtitle_);

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);

    std::ostringstream lineOne;
    lineOne << "Kills: " << resources.kills << "  Grafts: " << resources.grafts
            << "  Research: " << resources.research << "  Bosses: " << runState.bossesDefeated;
    DrawText(renderer_, modal.x + 54.0F, modal.y + 126.0F, SDL_Color{190, 196, 188, 255},
             lineOne.str());

    std::ostringstream lineTwo;
    lineTwo << "Essence this run: " << resources.essence << "  Banked Essence: " << bankedEssence_
            << "  Last Payout: " << lastPayout_;
    DrawText(renderer_, modal.x + 54.0F, modal.y + 150.0F, SDL_Color{222, 204, 166, 255},
             lineTwo.str());

    DrawText(
        renderer_, modal.x + 54.0F, modal.y + 206.0F, SDL_Color{186, 182, 176, 255},
        sceneState_ == SceneState::Victory
            ? "The demo run is complete. Start another run or return to the title screen."
            : "Retry immediately, or return to the title screen to review your banked Essence.");
    if (sceneState_ == SceneState::Defeat) {
        DrawText(renderer_, modal.x + 54.0F, modal.y + 228.0F, SDL_Color{214, 142, 132, 255},
                 "Press E at any time on this screen to dissolve the failed run.");
    }

    FillRect(renderer_, primaryButton, SDL_Color{108, 136, 104, 255});
    DrawRect(renderer_, primaryButton, SDL_Color{236, 232, 224, 255});
    DrawText(renderer_, primaryButton.x + 120.0F, primaryButton.y + 15.0F,
             SDL_Color{18, 18, 18, 255}, "Start New Run");

    FillRect(renderer_, secondaryButton, SDL_Color{86, 92, 122, 255});
    DrawRect(renderer_, secondaryButton, SDL_Color{236, 232, 224, 255});
    DrawText(renderer_, secondaryButton.x + 132.0F, secondaryButton.y + 15.0F,
             SDL_Color{18, 18, 18, 255}, "Return to Title");
}
