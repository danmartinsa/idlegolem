#include <sstream>

#include "fleshgolem_game.h"

using namespace fleshgolem_internal;

namespace {

std::string BuildSkillNodeSummary(const SkillNodeDefinition& definition) {
    std::ostringstream line;
    const SkillEffect& effect = definition.effectPerRank;

    bool first = true;
    const auto append = [&line, &first](const std::string& text) {
        if (!first) {
            line << "  ";
        }
        line << text;
        first = false;
    };

    if (effect.attributeBonus.might > 0.0F) {
        append("Mgt +" + FormatNumber(effect.attributeBonus.might));
    }
    if (effect.attributeBonus.vitality > 0.0F) {
        append("Vit +" + FormatNumber(effect.attributeBonus.vitality));
    }
    if (effect.attributeBonus.agility > 0.0F) {
        append("Agi +" + FormatNumber(effect.attributeBonus.agility));
    }
    if (effect.attributeBonus.reason > 0.0F) {
        append("Rsn +" + FormatNumber(effect.attributeBonus.reason));
    }
    if (effect.attributeBonus.instinct > 0.0F) {
        append("Ins +" + FormatNumber(effect.attributeBonus.instinct));
    }
    if (effect.attributeBonus.corruption > 0.0F) {
        append("Cor +" + FormatNumber(effect.attributeBonus.corruption));
    }
    if (effect.attackBonus > 0.0F) {
        append("Atk +" + FormatNumber(effect.attackBonus));
    }
    if (effect.healthBonus > 0.0F) {
        append("HP +" + FormatNumber(effect.healthBonus));
    }
    if (effect.attackIntervalBonus < 0.0F) {
        append("Spd +" + FormatNumber(-effect.attackIntervalBonus));
    }
    if (effect.critChanceBonus > 0.0F) {
        append("Crit +" + std::to_string(static_cast<int>(
                              std::round(effect.critChanceBonus * 100.0F))));
    }
    if (effect.armorBonus > 0.0F) {
        append("Arm +" + FormatNumber(effect.armorBonus));
    }
    if (effect.evasionBonus > 0.0F) {
        append("Eva +" + std::to_string(static_cast<int>(
                             std::round(effect.evasionBonus * 100.0F))));
    }
    if (effect.stabilityCapacityBonus > 0.0F) {
        append("Stab +" + FormatNumber(effect.stabilityCapacityBonus));
    }
    if (effect.decayMultiplierBonus < 0.0F) {
        append("Decay -" +
               std::to_string(static_cast<int>(
                   std::round(-effect.decayMultiplierBonus * 100.0F))) +
               "%");
    }
    if (effect.harvestMultiplierBonus > 0.0F) {
        append("Yield +" +
               std::to_string(static_cast<int>(
                   std::round(effect.harvestMultiplierBonus * 100.0F))) +
               "%");
    }
    if (effect.harvestSpeedMultiplierBonus > 0.0F) {
        append("Harvest +" +
               std::to_string(static_cast<int>(
                   std::round(effect.harvestSpeedMultiplierBonus * 100.0F))) +
               "%");
    }
    if (effect.levelHealPercentBonus > 0.0F) {
        append("Lvl heal " +
               std::to_string(static_cast<int>(
                   std::round(effect.levelHealPercentBonus * 100.0F))) +
               "%");
    }
    if (effect.bonusEssencePerKill > 0) {
        append("Ess/kill +" + std::to_string(effect.bonusEssencePerKill));
    }
    if (effect.researchOnEliteKill > 0) {
        append("Res/elite +" + std::to_string(effect.researchOnEliteKill));
    }

    return line.str();
}

}  // namespace

void FleshgolemGame::RenderBackdrop() const {
    const SDL_FRect fullScreen{0.0F, 0.0F, kWindowWidth, kWindowHeight};

    if (sceneState_ == SceneState::Title) {
        FillRect(renderer_, fullScreen, SDL_Color{16, 10, 16, 255});
        if (titleBannerTexture_ != nullptr) {
            SDL_RenderTexture(renderer_, titleBannerTexture_, nullptr,
                              &fullScreen);
        }
        FillRect(renderer_, fullScreen, SDL_Color{8, 6, 10, 120});
        return;
    }

    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);

    FillRect(renderer_, fullScreen, zone.skyTop);
    if (zoneBackdropTextures_[static_cast<std::size_t>(runState.zoneIndex)] !=
        nullptr) {
        SDL_RenderTexture(
            renderer_,
            zoneBackdropTextures_[static_cast<std::size_t>(runState.zoneIndex)],
            nullptr, &fullScreen);
    }
    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, kWindowWidth, 220.0F},
             zone.skyBottom);
    FillRect(renderer_, fullScreen, SDL_Color{0, 0, 0, 44});
}

void FleshgolemGame::RenderTitleScreen() const {
    const SDL_FRect panel{200.0F, 98.0F, 880.0F, 510.0F};
    const SDL_FRect leftColumn{panel.x + 28.0F, panel.y + 34.0F, 386.0F,
                               360.0F};
    const SDL_FRect rightColumn{panel.x + 446.0F, panel.y + 34.0F, 406.0F,
                                278.0F};
    const SDL_FRect footerPanel{panel.x + 446.0F, panel.y + 332.0F, 406.0F,
                                168.0F};
    const SDL_FRect startButton = TitleStartButtonRect();

    DrawPanelChrome(renderer_, panel, SDL_Color{24, 18, 24, 228},
                    SDL_Color{206, 188, 174, 255}, SDL_Color{146, 74, 86, 255});
    DrawPanelChrome(renderer_, leftColumn, SDL_Color{28, 20, 28, 220},
                    SDL_Color{124, 104, 108, 255},
                    SDL_Color{178, 118, 96, 255});
    DrawPanelChrome(renderer_, rightColumn, SDL_Color{24, 22, 30, 220},
                    SDL_Color{112, 106, 128, 255},
                    SDL_Color{110, 136, 174, 255});
    DrawPanelChrome(renderer_, footerPanel, SDL_Color{26, 22, 28, 220},
                    SDL_Color{112, 106, 128, 255},
                    SDL_Color{96, 134, 116, 255});

    DrawTag(
        renderer_,
        SDL_FRect{leftColumn.x + 12.0F, leftColumn.y + 16.0F, 134.0F, 24.0F},
        SDL_Color{184, 120, 98, 255}, SDL_Color{236, 228, 220, 255},
        "SDL3 + EnTT Demo");

    DrawText(renderer_, leftColumn.x + 16.0F, leftColumn.y + 54.0F,
             SDL_Color{242, 226, 214, 255}, "FLESHGOLEM");
    DrawWrappedText(renderer_, leftColumn.x + 16.0F, leftColumn.y + 82.0F,
                    leftColumn.w - 32.0F, 2, 16.0F,
                    SDL_Color{206, 188, 176, 255},
                    "A run-based grafting crawl with auto-combat.");

    DrawText(renderer_, leftColumn.x + 16.0F, leftColumn.y + 124.0F,
             SDL_Color{220, 208, 198, 255}, "This demo now includes:");
    DrawWrappedText(renderer_, leftColumn.x + 24.0F, leftColumn.y + 148.0F,
                    leftColumn.w - 48.0F, 2, 16.0F,
                    SDL_Color{190, 184, 178, 255},
                    "- title flow, polish, and clearer visual hierarchy");
    DrawWrappedText(
        renderer_, leftColumn.x + 24.0F, leftColumn.y + 180.0F,
        leftColumn.w - 48.0F, 2, 16.0F, SDL_Color{190, 184, 178, 255},
        "- three zones, elite waves, bosses, and level-based skill growth");
    DrawWrappedText(renderer_, leftColumn.x + 24.0F, leftColumn.y + 212.0F,
                    leftColumn.w - 48.0F, 2, 16.0F,
                    SDL_Color{190, 184, 178, 255},
                    "- meta Essence, Research, generated assets, and audio");
    DrawWrappedText(renderer_, leftColumn.x + 24.0F, leftColumn.y + 244.0F,
                    leftColumn.w - 48.0F, 2, 16.0F,
                    SDL_Color{190, 184, 178, 255},
                    "- the original auto-combat and graft loop underneath");

    DrawText(renderer_, leftColumn.x + 16.0F, leftColumn.y + 290.0F,
             SDL_Color{222, 204, 166, 255}, "Run summary");
    std::ostringstream metaLine;
    metaLine << "Banked Essence " << bankedEssence_ << "   Runs "
             << completedRuns_ << "   Last payout " << lastPayout_;
    DrawWrappedText(renderer_, leftColumn.x + 16.0F, leftColumn.y + 314.0F,
                    leftColumn.w - 32.0F, 2, 16.0F,
                    SDL_Color{214, 198, 172, 255}, metaLine.str());
    DrawWrappedText(renderer_, leftColumn.x + 16.0F, leftColumn.y + 346.0F,
                    leftColumn.w - 32.0F, 2, 16.0F,
                    SDL_Color{188, 196, 188, 255},
                    "Space pause  K tree  R restart  E dissolve  N scout");

    DrawText(renderer_, rightColumn.x + 16.0F, rightColumn.y + 18.0F,
             SDL_Color{232, 220, 214, 255}, "Demo structure");
    DrawTag(
        renderer_,
        SDL_FRect{rightColumn.x + 16.0F, rightColumn.y + 50.0F, 112.0F, 24.0F},
        SDL_Color{110, 144, 110, 255}, SDL_Color{230, 228, 222, 255},
        "1. Start weak");
    DrawTag(
        renderer_,
        SDL_FRect{rightColumn.x + 146.0F, rightColumn.y + 50.0F, 118.0F, 24.0F},
        SDL_Color{122, 104, 162, 255}, SDL_Color{230, 228, 222, 255},
        "2. Harvest");
    DrawTag(
        renderer_,
        SDL_FRect{rightColumn.x + 282.0F, rightColumn.y + 50.0F, 110.0F, 24.0F},
        SDL_Color{184, 128, 72, 255}, SDL_Color{230, 228, 222, 255},
        "3. Upgrade");

    DrawWrappedText(
        renderer_, rightColumn.x + 16.0F, rightColumn.y + 98.0F,
        rightColumn.w - 32.0F, 3, 20.0F, SDL_Color{196, 188, 182, 255},
        "March through the Charnel Pits, Ash Chapel, and Black Cathedral, "
        "choosing one "
        "skill path as level-ups grant points to spend in the tree.");
    DrawWrappedText(renderer_, rightColumn.x + 16.0F, rightColumn.y + 178.0F,
                    rightColumn.w - 32.0F, 2, 16.0F,
                    SDL_Color{170, 186, 204, 255},
                    "Mouse and keyboard are both supported.");
    DrawWrappedText(
        renderer_, rightColumn.x + 16.0F, rightColumn.y + 210.0F,
        rightColumn.w - 32.0F, 2, 16.0F, SDL_Color{170, 186, 204, 255},
        "Spend points with mouse clicks or keyboard navigation in the tree.");
    DrawWrappedText(renderer_, rightColumn.x + 16.0F, rightColumn.y + 242.0F,
                    rightColumn.w - 32.0F, 2, 16.0F,
                    SDL_Color{170, 186, 204, 255},
                    "Runs can end in victory, collapse, or dissolve.");

    DrawText(renderer_, footerPanel.x + 16.0F, footerPanel.y + 18.0F,
             SDL_Color{232, 220, 214, 255}, "Awaken the run");
    DrawWrappedText(renderer_, footerPanel.x + 16.0F, footerPanel.y + 44.0F,
                    footerPanel.w - 32.0F, 2, 16.0F,
                    SDL_Color{184, 190, 182, 255},
                    "Start a fresh homunculus and march into the first zone.");

    DrawButtonChrome(renderer_, startButton, SDL_Color{108, 136, 104, 255},
                     SDL_Color{236, 232, 224, 255},
                     SDL_Color{164, 204, 152, 255}, "Start Demo", true);

    DrawTextFit(
        renderer_, footerPanel.x + 16.0F, footerPanel.y + 132.0F,
        footerPanel.w - 32.0F, SDL_Color{174, 168, 160, 255},
        "Press Enter or click the button to awaken a fresh homunculus.");
}

void FleshgolemGame::RenderResourcePanel(const SDL_FRect& panel) const {
    DrawPanelChrome(renderer_, panel, SDL_Color{32, 24, 34, 240},
                    SDL_Color{118, 96, 118, 255}, SDL_Color{144, 92, 118, 255});

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);
    const StabilityState& stability = registry_.get<StabilityState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);

    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F,
             SDL_Color{234, 214, 214, 255}, "FLESHGOLEM DEMO");
    DrawTextFit(renderer_, panel.x + 180.0F, panel.y + 12.0F, 420.0F,
                SDL_Color{194, 188, 182, 255},
                "Space pause  K tree  R restart  E dissolve  N scout");
    DrawTag(renderer_,
            SDL_FRect{panel.x + 940.0F, panel.y + 10.0F, 146.0F, 24.0F},
            zone.accent, SDL_Color{236, 232, 224, 255}, zone.name);
    DrawTag(renderer_,
            SDL_FRect{panel.x + 1096.0F, panel.y + 10.0F, 120.0F, 24.0F},
            SDL_Color{98, 106, 126, 255}, SDL_Color{236, 232, 224, 255},
            sceneState_ == SceneState::SkillTree ? "Skill Tree"
            : paused_                            ? "Paused"
                                                 : "Run Active",
            SDL_Color{232, 232, 228, 255});

    std::ostringstream resourceLine;
    resourceLine << "Wave " << (runState.zoneEncounter + 1) << "/"
                 << runState.encountersPerZone << "   Lv " << runState.level
                 << "   XP " << runState.experience << "/"
                 << runState.experienceToNextLevel << "   SP "
                 << runState.skillPoints << "   K " << resources.kills
                 << "   Bio " << resources.biomass << "   Bone "
                 << resources.boneMeal << "   Res " << resources.research;
    DrawTextFit(renderer_, panel.x + 16.0F, panel.y + 34.0F, panel.w - 32.0F,
                SDL_Color{206, 182, 178, 255}, resourceLine.str());

    std::ostringstream statusLine;
    statusLine << "Grafts " << resources.grafts << "   Salvaged "
               << resources.salvaged << "   Spent " << runState.spentSkillPoints
               << "   Bosses " << runState.bossesDefeated << "   RunEss "
               << resources.essence << "   Bank " << bankedEssence_
               << "   Stab " << FormatNumber(stability.demand) << "/"
               << FormatNumber(stability.capacity);
    DrawTextFit(renderer_, panel.x + 16.0F, panel.y + 54.0F, panel.w - 32.0F,
                stability.overload > 0.0F ? SDL_Color{214, 108, 98, 255}
                                          : SDL_Color{154, 196, 142, 255},
                statusLine.str());
}

void FleshgolemGame::RenderPlayerPanel(const SDL_FRect& panel) const {
    DrawPanelChrome(renderer_, panel, SDL_Color{28, 24, 30, 240},
                    SDL_Color{92, 82, 98, 255}, SDL_Color{138, 104, 148, 255});

    const Name& name = registry_.get<Name>(player_);
    const Health& health = registry_.get<Health>(player_);
    const Attributes& attributes = registry_.get<Attributes>(player_);
    const CombatStats& stats = registry_.get<CombatStats>(player_);
    const StabilityState& stability = registry_.get<StabilityState>(player_);
    const Body& body = registry_.get<Body>(player_);

    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F,
             SDL_Color{232, 216, 216, 255}, name.value);
    DrawTag(renderer_,
            SDL_FRect{panel.x + 330.0F, panel.y + 10.0F, 118.0F, 24.0F},
            SDL_Color{92, 104, 126, 255}, SDL_Color{232, 228, 220, 255},
            "Body status", SDL_Color{232, 232, 228, 255});
    DrawBar(renderer_,
            SDL_FRect{panel.x + 16.0F, panel.y + 34.0F, 220.0F, 12.0F},
            health.current / std::max(1.0F, health.maximum),
            SDL_Color{170, 72, 76, 255}, SDL_Color{70, 36, 44, 255});

    std::ostringstream hpLine;
    hpLine << "HP " << static_cast<int>(std::round(health.current)) << "/"
           << static_cast<int>(std::round(health.maximum));
    DrawTextFit(renderer_, panel.x + 246.0F, panel.y + 36.0F, 180.0F,
                SDL_Color{214, 196, 192, 255}, hpLine.str());

    std::ostringstream statLineOne;
    statLineOne << "Mgt " << FormatNumber(attributes.might) << "  Vit "
                << FormatNumber(attributes.vitality) << "  Agi "
                << FormatNumber(attributes.agility);
    DrawTextFit(renderer_, panel.x + 16.0F, panel.y + 60.0F, 430.0F,
                SDL_Color{186, 194, 182, 255}, statLineOne.str());

    std::ostringstream statLineTwo;
    statLineTwo << "Rsn " << FormatNumber(attributes.reason) << "  Ins "
                << FormatNumber(attributes.instinct) << "  Cor "
                << FormatNumber(attributes.corruption);
    DrawTextFit(renderer_, panel.x + 16.0F, panel.y + 78.0F, 430.0F,
                SDL_Color{186, 194, 182, 255}, statLineTwo.str());

    std::ostringstream combatLine;
    combatLine << "Atk " << FormatNumber(stats.attackPower) << "  Spd "
               << FormatNumber(1.0F / stats.attackInterval, 2) << "  Crit "
               << static_cast<int>(std::round(stats.critChance * 100.0F))
               << "  Arm " << FormatNumber(stats.armor) << "  Eva "
               << static_cast<int>(std::round(stats.evasion * 100.0F));
    DrawTextFit(renderer_, panel.x + 16.0F, panel.y + 96.0F, 430.0F,
                SDL_Color{172, 186, 202, 255}, combatLine.str());

    std::ostringstream stabilityLine;
    stabilityLine << "Stab " << FormatNumber(stability.demand) << "/"
                  << FormatNumber(stability.capacity) << "  Harv x"
                  << FormatNumber(runUpgrades_.harvestMultiplier, 2)
                  << "  Decay x"
                  << FormatNumber(runUpgrades_.decayMultiplier, 2);
    if (stability.overload > 0.0F) {
        stabilityLine << "  OVERLOAD +" << FormatNumber(stability.overload);
    }
    DrawTextFit(renderer_, panel.x + 16.0F, panel.y + 114.0F, 430.0F,
                stability.overload > 0.0F ? SDL_Color{214, 108, 98, 255}
                                          : SDL_Color{144, 182, 144, 255},
                stabilityLine.str());

    FillRect(renderer_,
             SDL_FRect{panel.x + 14.0F, panel.y + 146.0F, 442.0F, 1.0F},
             SDL_Color{96, 82, 98, 255});

    const auto slotRects = BuildSlotRects(panel);
    for (std::size_t index = 0; index < kSlotCount; ++index) {
        const SlotType slot = kAllSlots[index];
        const entt::entity partEntity = body.slots[index];
        SDL_Color fill{58, 54, 64, 255};
        std::string freshnessText = "--";

        if (partEntity != entt::null && registry_.valid(partEntity)) {
            const BodyPart& part = registry_.get<BodyPart>(partEntity);
            fill = part.color;
            freshnessText =
                std::to_string(static_cast<int>(std::round(part.freshness)));
        }

        FillRect(renderer_, slotRects[index], fill);
        DrawRect(renderer_, slotRects[index], SDL_Color{214, 214, 214, 255});
        DrawText(renderer_, slotRects[index].x + 6.0F,
                 slotRects[index].y + 6.0F, SDL_Color{18, 18, 18, 255},
                 SlotShortLabel(slot));
        DrawText(renderer_, slotRects[index].x + 6.0F,
                 slotRects[index].y + slotRects[index].h - 14.0F,
                 SDL_Color{18, 18, 18, 255}, freshnessText);
    }

    DrawText(renderer_, panel.x + 278.0F, panel.y + 156.0F,
             SDL_Color{212, 202, 196, 255}, "Equipped anatomy");
    float textY = panel.y + 176.0F;
    for (const SlotType slot : kAllSlots) {
        const entt::entity partEntity = body.slots[SlotIndex(slot)];
        std::ostringstream headerLine;
        headerLine << SlotShortLabel(slot);
        SDL_Color textColor{178, 168, 168, 255};
        std::string detailLine = "empty";
        if (partEntity != entt::null && registry_.valid(partEntity)) {
            const BodyPart& part = registry_.get<BodyPart>(partEntity);
            headerLine << "  " << static_cast<int>(std::round(part.freshness))
                       << "%";
            detailLine = part.name;
            textColor = part.color;
        }
        DrawTextFit(renderer_, panel.x + 278.0F, textY, 176.0F, textColor,
                    headerLine.str());
        DrawWrappedText(renderer_, panel.x + 278.0F, textY + 14.0F, 176.0F, 2,
                        14.0F, textColor, detailLine);
        textY += 30.0F;
    }
}

void FleshgolemGame::RenderEncounterPanel(const SDL_FRect& panel) const {
    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);
    DrawPanelChrome(renderer_, panel, SDL_Color{30, 22, 26, 240},
                    SDL_Color{102, 80, 90, 255}, zone.accent);

    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F,
             SDL_Color{232, 216, 216, 255}, zone.name);
    DrawTag(renderer_,
            SDL_FRect{panel.x + 134.0F, panel.y + 10.0F, 104.0F, 24.0F},
            zone.accent, SDL_Color{232, 228, 220, 255}, "Encounter");
    DrawWrappedText(renderer_, panel.x + 250.0F, panel.y + 12.0F,
                    panel.w - 266.0F, 2, 16.0F, SDL_Color{190, 184, 178, 255},
                    zone.subtitle);

    const SDL_FRect laneRect{panel.x + 16.0F, panel.y + 54.0F, panel.w - 32.0F,
                             194.0F};
    RenderLane(laneRect);

    DrawBar(renderer_,
            SDL_FRect{panel.x + 18.0F, panel.y + 258.0F, 340.0F, 10.0F},
            static_cast<float>(runState.zoneEncounter) /
                static_cast<float>(std::max(1, runState.encountersPerZone)),
            zone.accent, SDL_Color{62, 52, 58, 255});
    DrawText(renderer_, panel.x + 366.0F, panel.y + 258.0F,
             SDL_Color{212, 202, 194, 255}, "Zone progress");

    if (currentEnemy_ != entt::null && registry_.valid(currentEnemy_)) {
        const Name& name = registry_.get<Name>(currentEnemy_);
        const Health& health = registry_.get<Health>(currentEnemy_);
        const Enemy& enemy = registry_.get<Enemy>(currentEnemy_);

        DrawTextFit(renderer_, panel.x + 18.0F, panel.y + 278.0F, 390.0F,
                    SDL_Color{220, 206, 194, 255}, name.value);
        DrawBar(renderer_,
                SDL_FRect{panel.x + 18.0F, panel.y + 296.0F, 260.0F, 12.0F},
                health.current / std::max(1.0F, health.maximum),
                SDL_Color{182, 86, 96, 255}, SDL_Color{84, 38, 48, 255});

        std::ostringstream line;
        line << "T" << enemy.tier << "  "
             << (combatJoined_ ? "CONTACT" : "APPROACHING");
        if (enemy.elite) {
            line << "  ELITE";
        }
        if (enemy.boss) {
            line << "  BOSS";
        }
        DrawTextFit(renderer_, panel.x + 18.0F, panel.y + 314.0F,
                    panel.w - 36.0F, SDL_Color{196, 188, 182, 255}, line.str());
    } else if (currentCorpse_ != entt::null &&
               registry_.valid(currentCorpse_)) {
        const Corpse& corpse = registry_.get<Corpse>(currentCorpse_);
        DrawTextFit(renderer_, panel.x + 18.0F, panel.y + 278.0F, 390.0F,
                    SDL_Color{208, 204, 194, 255},
                    registry_.get<Name>(currentCorpse_).value);
        DrawBar(renderer_,
                SDL_FRect{panel.x + 18.0F, panel.y + 296.0F, 260.0F, 12.0F},
                1.0F - corpse.harvestTimer /
                           std::max(0.01F, corpse.maxHarvestTimer),
                SDL_Color{112, 172, 96, 255}, SDL_Color{50, 74, 46, 255});
        DrawWrappedText(
            renderer_, panel.x + 18.0F, panel.y + 314.0F, panel.w - 36.0F, 2,
            16.0F, SDL_Color{180, 196, 162, 255},
            corpse.boss
                ? "Boss corpse: harvesting pushes the run into the next phase."
                : "Auto-harvesting the corpse for resources, salvage, and "
                  "momentum.");
    } else {
        DrawWrappedText(
            renderer_, panel.x + 18.0F, panel.y + 278.0F, panel.w - 36.0F, 2,
            16.0F, SDL_Color{208, 198, 188, 255},
            "The homunculus keeps walking while the next enemy approaches.");
        DrawWrappedText(renderer_, panel.x + 18.0F, panel.y + 310.0F,
                        panel.w - 36.0F, 2, 16.0F,
                        SDL_Color{176, 170, 164, 255},
                        "Every level grants a point for the skill tree.");
    }
}

void FleshgolemGame::RenderLane(const SDL_FRect& laneRect) const {
    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);
    const float groundY = laneRect.y + kLaneGroundOffsetY;

    FillRect(renderer_, laneRect, SDL_Color{28, 20, 26, 255});
    if (zoneBackdropTextures_[static_cast<std::size_t>(runState.zoneIndex)] !=
        nullptr) {
        SDL_RenderTexture(
            renderer_,
            zoneBackdropTextures_[static_cast<std::size_t>(runState.zoneIndex)],
            nullptr, &laneRect);
    }
    FillRect(renderer_,
             SDL_FRect{laneRect.x, laneRect.y + 18.0F, laneRect.w,
                       groundY - laneRect.y - 18.0F},
             zone.skyBottom);
    FillRect(renderer_,
             SDL_FRect{laneRect.x, groundY, laneRect.w,
                       laneRect.h - (groundY - laneRect.y)},
             zone.ground);

    for (int stripe = 0; stripe < 7; ++stripe) {
        const float stripeX =
            laneRect.x + 18.0F + static_cast<float>(stripe) * 98.0F;
        FillRect(renderer_, SDL_FRect{stripeX, groundY + 22.0F, 44.0F, 4.0F},
                 SDL_Color{96, 70, 64, 255});
    }

    DrawRect(renderer_, laneRect, SDL_Color{136, 112, 106, 255});
    DrawRect(renderer_,
             SDL_FRect{laneRect.x + kLaneEnemyContactX - 10.0F,
                       laneRect.y + 22.0F, 12.0F, laneRect.h - 40.0F},
             zone.accent);

    const SDL_FRect playerRect{laneRect.x + kLanePlayerX,
                               groundY - kPlayerRenderHeight,
                               kPlayerRenderWidth, kPlayerRenderHeight};
    if (playerSpriteSheet_ != nullptr) {
        const SDL_FRect sourceRect{
            static_cast<float>(playerAnimationFrame_ * 64), 0.0F, 64.0F, 64.0F};
        SDL_RenderTexture(renderer_, playerSpriteSheet_, &sourceRect,
                          &playerRect);
    } else {
        FillRect(renderer_, playerRect, SDL_Color{120, 72, 86, 255});
        DrawRect(renderer_, playerRect, SDL_Color{220, 216, 210, 255});
    }

    DrawWrappedText(renderer_, laneRect.x + 24.0F, laneRect.y + 10.0F,
                    laneRect.w - 48.0F, 2, 16.0F, SDL_Color{210, 204, 198, 255},
                    "Walk in place, gain levels, and open the skill tree "
                    "whenever points are ready.");

    const Health& playerHealth = registry_.get<Health>(player_);
    DrawBar(renderer_,
            SDL_FRect{playerRect.x + 8.0F, playerRect.y - 18.0F,
                      playerRect.w - 16.0F, 10.0F},
            playerHealth.current / std::max(1.0F, playerHealth.maximum),
            SDL_Color{170, 76, 78, 255}, SDL_Color{76, 34, 40, 255});

    if (currentEnemy_ != entt::null && registry_.valid(currentEnemy_)) {
        const Enemy& enemy = registry_.get<Enemy>(currentEnemy_);
        const EnemyTemplate enemyTemplate = EnemyTemplateFor(enemy.kind);
        const Health& enemyHealth = registry_.get<Health>(currentEnemy_);

        const SDL_FRect enemyRect{laneRect.x + enemyLaneX_,
                                  groundY - kEnemyRenderHeight,
                                  kEnemyRenderWidth, kEnemyRenderHeight};
        SDL_Texture* enemySprite =
            enemySpriteSheets_[EnemyKindIndex(enemy.kind)];
        if (enemySprite != nullptr) {
            const SDL_FRect sourceRect{
                static_cast<float>(playerAnimationFrame_ * 64), 0.0F, 64.0F,
                64.0F};
            SDL_RenderTexture(renderer_, enemySprite, &sourceRect, &enemyRect);
            if (enemy.elite || enemy.boss) {
                DrawRect(renderer_,
                         SDL_FRect{enemyRect.x + 12.0F, enemyRect.y + 8.0F,
                                   enemyRect.w - 24.0F, enemyRect.h - 16.0F},
                         enemy.boss ? SDL_Color{214, 220, 126, 255}
                                    : SDL_Color{226, 194, 88, 255});
            }
        } else {
            RenderEnemyFigure(renderer_, enemyRect, enemyTemplate.color,
                              enemy.elite || enemy.boss);
        }
        DrawBar(renderer_,
                SDL_FRect{enemyRect.x + 8.0F, enemyRect.y - 18.0F,
                          enemyRect.w - 16.0F, 10.0F},
                enemyHealth.current / std::max(1.0F, enemyHealth.maximum),
                SDL_Color{184, 94, 98, 255}, SDL_Color{82, 38, 44, 255});

        DrawText(
            renderer_, enemyRect.x + 18.0F, enemyRect.y + enemyRect.h + 8.0F,
            SDL_Color{220, 214, 204, 255},
            enemy.boss ? "BOSS" : (combatJoined_ ? "ENGAGED" : "APPROACHING"));
    } else if (currentCorpse_ != entt::null &&
               registry_.valid(currentCorpse_)) {
        const Corpse& corpse = registry_.get<Corpse>(currentCorpse_);
        const EnemyTemplate enemyTemplate = EnemyTemplateFor(corpse.source);
        const SDL_FRect corpseRect{laneRect.x + corpseLaneX_, groundY - 48.0F,
                                   kEnemyRenderWidth, 52.0F};
        RenderCorpsePile(renderer_, corpseRect, enemyTemplate.color);
        DrawText(renderer_, corpseRect.x + 24.0F, corpseRect.y - 18.0F,
                 SDL_Color{198, 212, 188, 255},
                 corpse.boss ? "HARVEST BOSS" : "HARVEST");
    } else {
        DrawText(renderer_, laneRect.x + kLaneEnemySpawnX - 66.0F,
                 laneRect.y + 34.0F, SDL_Color{182, 182, 192, 255}, "ENTRY");
    }
}

void FleshgolemGame::RenderEventLog(const SDL_FRect& panel) const {
    DrawPanelChrome(renderer_, panel, SDL_Color{24, 22, 24, 240},
                    SDL_Color{90, 82, 88, 255}, SDL_Color{108, 90, 116, 255});
    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F,
             SDL_Color{224, 214, 206, 255}, "Recent events");

    float y = panel.y + 34.0F;
    int lineIndex = 0;
    for (const std::string& line : eventLog_) {
        FillRect(renderer_,
                 SDL_FRect{panel.x + 12.0F, y - 4.0F, panel.w - 24.0F, 16.0F},
                 lineIndex == 0 ? SDL_Color{72, 56, 66, 120}
                                : SDL_Color{42, 36, 44, 92});
        const SDL_Color color = (lineIndex == 0)
                                    ? SDL_Color{220, 210, 198, 255}
                                    : SDL_Color{166, 166, 166, 255};
        DrawTextFit(renderer_, panel.x + 16.0F, y, panel.w - 32.0F, color,
                    line);
        y += 18.0F;
        ++lineIndex;
    }
}

void FleshgolemGame::RenderPauseOverlay() const {
    const SDL_FRect modal = PauseModalRect();
    const SDL_FRect resumeButton = ResumeButtonRect();
    const SDL_FRect restartButton = RestartButtonRect();

    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, kWindowWidth, kWindowHeight},
             SDL_Color{0, 0, 0, 150});
    DrawPanelChrome(renderer_, modal, SDL_Color{40, 28, 36, 245},
                    SDL_Color{188, 172, 168, 255},
                    SDL_Color{146, 104, 122, 255});

    DrawText(renderer_, modal.x + 148.0F, modal.y + 32.0F,
             SDL_Color{238, 226, 220, 255}, "PAUSED");
    DrawTextFit(renderer_, modal.x + 54.0F, modal.y + 74.0F, modal.w - 108.0F,
                SDL_Color{208, 198, 192, 255},
                "The lane, combat, harvesting, and decay are frozen.");
    DrawTextFit(renderer_, modal.x + 74.0F, modal.y + 94.0F, modal.w - 148.0F,
                SDL_Color{176, 168, 162, 255},
                "Press Space or click a button to continue.");

    DrawButtonChrome(renderer_, resumeButton, SDL_Color{92, 132, 108, 255},
                     SDL_Color{228, 228, 220, 255},
                     SDL_Color{166, 202, 164, 255}, "Resume", true);

    DrawButtonChrome(renderer_, restartButton, SDL_Color{142, 84, 90, 255},
                     SDL_Color{228, 228, 220, 255},
                     SDL_Color{206, 138, 144, 255}, "Restart");
}

void FleshgolemGame::RenderSkillTreeOverlay() const {
    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, kWindowWidth, kWindowHeight},
             SDL_Color{0, 0, 0, 168});

    const RunState& runState = registry_.get<RunState>(player_);
    const SDL_FRect modal = SkillTreeModalRect();
    const SDL_FRect graph = SkillTreeGraphRect();
    const SDL_FRect details = SkillTreeDetailsRect();
    const SDL_FRect spendButton = SkillSpendButtonRect();
    const SDL_FRect closeButton = SkillCloseButtonRect();
    const SkillNodeDefinition& selectedDefinition =
        SkillDefinition(selectedSkillIndex_);
    const int selectedRank =
        runState.skillRanks[static_cast<std::size_t>(selectedSkillIndex_)];
    const bool selectedUnlocked =
        SkillPrerequisitesMet(selectedDefinition, runState.skillRanks);
    const bool selectedCanSpend = selectedUnlocked &&
                                  selectedRank < selectedDefinition.maxRank &&
                                  runState.skillPoints > 0;

    DrawPanelChrome(renderer_, modal, SDL_Color{26, 20, 28, 245},
                    SDL_Color{196, 188, 180, 255},
                    SDL_Color{146, 104, 162, 255});
    DrawPanelChrome(renderer_, graph, SDL_Color{24, 22, 28, 238},
                    SDL_Color{98, 90, 112, 255}, SDL_Color{104, 126, 174, 255});
    DrawPanelChrome(renderer_, details, SDL_Color{28, 24, 30, 238},
                    SDL_Color{98, 90, 112, 255}, SDL_Color{180, 126, 92, 255});

    DrawText(renderer_, modal.x + 26.0F, modal.y + 18.0F,
             SDL_Color{242, 232, 224, 255}, "SKILL TREE");

    std::ostringstream levelLine;
    levelLine << "Level " << runState.level << "   XP " << runState.experience
              << "/" << runState.experienceToNextLevel << "   Unspent points "
              << runState.skillPoints;
    DrawTextFit(renderer_, modal.x + 170.0F, modal.y + 20.0F, 560.0F,
                SDL_Color{212, 204, 196, 255}, levelLine.str());
    DrawTextFit(renderer_, modal.x + 26.0F, modal.y + 44.0F, modal.w - 52.0F,
                SDL_Color{180, 184, 192, 255},
                "Arrow keys move, Enter spends, K or Esc closes. Mouse can "
                "select nodes and use "
                "the buttons.");

    const float xpRatio =
        static_cast<float>(runState.experience) /
        static_cast<float>(std::max(1, runState.experienceToNextLevel));
    DrawBar(renderer_,
            SDL_FRect{modal.x + 738.0F, modal.y + 22.0F, 294.0F, 10.0F},
            xpRatio, SDL_Color{110, 174, 126, 255}, SDL_Color{52, 56, 62, 255});

    for (int skillIndex = 0; skillIndex < static_cast<int>(kSkillNodeCount);
         ++skillIndex) {
        const SkillNodeDefinition& definition = SkillDefinition(skillIndex);
        const SDL_FPoint start = SkillNodeCenter(skillIndex);
        for (int prerequisiteIndex = 0;
             prerequisiteIndex < definition.prerequisiteCount;
             ++prerequisiteIndex) {
            const int prerequisite =
                definition
                    .prerequisites[static_cast<std::size_t>(prerequisiteIndex)];
            if (prerequisite < 0) {
                continue;
            }
            const SDL_FPoint end = SkillNodeCenter(prerequisite);
            const bool active =
                runState.skillRanks[static_cast<std::size_t>(prerequisite)] > 0;
            const SDL_Color lineColor = active ? SDL_Color{188, 174, 128, 255}
                                               : SDL_Color{76, 72, 82, 255};
            const SDL_FRect horizontal{std::min(start.x, end.x), start.y - 1.0F,
                                       std::fabs(start.x - end.x), 3.0F};
            const SDL_FRect vertical{end.x - 1.0F, std::min(start.y, end.y),
                                     3.0F, std::fabs(start.y - end.y)};
            FillRect(renderer_, horizontal, lineColor);
            FillRect(renderer_, vertical, lineColor);
        }
    }

    for (int skillIndex = 0; skillIndex < static_cast<int>(kSkillNodeCount);
         ++skillIndex) {
        const SkillNodeDefinition& definition = SkillDefinition(skillIndex);
        const SDL_FRect nodeRect = SkillNodeRect(skillIndex);
        const int rank =
            runState.skillRanks[static_cast<std::size_t>(skillIndex)];
        const bool unlocked =
            SkillPrerequisitesMet(definition, runState.skillRanks);
        const bool maxed = rank >= definition.maxRank;
        const bool selected = skillIndex == selectedSkillIndex_;

        SDL_Color fill =
            unlocked ? SDL_Color{32, 28, 34, 236} : SDL_Color{18, 18, 22, 236};
        SDL_Color border =
            unlocked ? definition.color : SDL_Color{88, 84, 96, 255};
        if (selected) {
            border = SDL_Color{242, 236, 224, 255};
        }
        if (maxed) {
            fill = SDL_Color{44, 34, 38, 236};
        }

        DrawPanelChrome(
            renderer_, nodeRect, fill, border,
            unlocked ? definition.color : SDL_Color{70, 68, 78, 255});
        DrawTag(renderer_,
                SDL_FRect{nodeRect.x + 10.0F, nodeRect.y + 10.0F, 42.0F, 18.0F},
                unlocked ? definition.color : SDL_Color{84, 84, 90, 255},
                unlocked ? definition.color : SDL_Color{84, 84, 90, 255},
                definition.shortName);
        DrawTextFit(renderer_, nodeRect.x + 60.0F, nodeRect.y + 12.0F,
                    nodeRect.w - 70.0F,
                    unlocked ? SDL_Color{236, 228, 220, 255}
                             : SDL_Color{152, 148, 156, 255},
                    definition.name);

        std::ostringstream rankLine;
        rankLine << "Rank " << rank << "/" << definition.maxRank;
        DrawTextFit(renderer_, nodeRect.x + 10.0F, nodeRect.y + 36.0F,
                    nodeRect.w - 20.0F,
                    maxed      ? SDL_Color{220, 202, 156, 255}
                    : unlocked ? SDL_Color{188, 194, 202, 255}
                               : SDL_Color{132, 130, 138, 255},
                    rankLine.str());

        DrawWrappedText(renderer_, nodeRect.x + 10.0F, nodeRect.y + 56.0F,
                        nodeRect.w - 20.0F, 2, 14.0F,
                        unlocked ? SDL_Color{174, 170, 164, 255}
                                 : SDL_Color{114, 112, 120, 255},
                        BuildSkillNodeSummary(definition));
    }

    DrawText(renderer_, details.x + 18.0F, details.y + 16.0F,
             SDL_Color{238, 230, 222, 255}, selectedDefinition.name);

    std::ostringstream detailRankLine;
    detailRankLine << "Rank " << selectedRank << "/"
                   << selectedDefinition.maxRank;
    DrawTextFit(renderer_, details.x + 18.0F, details.y + 40.0F,
                details.w - 36.0F, SDL_Color{192, 198, 208, 255},
                detailRankLine.str());

    DrawWrappedText(renderer_, details.x + 18.0F, details.y + 68.0F,
                    details.w - 36.0F, 4, 16.0F, SDL_Color{210, 202, 194, 255},
                    selectedDefinition.description);
    DrawWrappedText(renderer_, details.x + 18.0F, details.y + 144.0F,
                    details.w - 36.0F, 4, 16.0F, SDL_Color{178, 196, 172, 255},
                    BuildSkillNodeSummary(selectedDefinition));

    std::ostringstream prereqLine;
    if (selectedDefinition.prerequisiteCount == 0) {
        prereqLine << "Prereqs: root branch";
    } else {
        prereqLine << "Prereqs:";
        for (int index = 0; index < selectedDefinition.prerequisiteCount;
             ++index) {
            const int prerequisite =
                selectedDefinition
                    .prerequisites[static_cast<std::size_t>(index)];
            if (prerequisite < 0) {
                continue;
            }
            prereqLine << (index == 0 ? " " : ", ")
                       << SkillDefinition(prerequisite).name;
        }
    }
    DrawWrappedText(renderer_, details.x + 18.0F, details.y + 220.0F,
                    details.w - 36.0F, 4, 16.0F,
                    selectedUnlocked ? SDL_Color{188, 188, 182, 255}
                                     : SDL_Color{214, 142, 132, 255},
                    prereqLine.str());

    DrawWrappedText(renderer_, details.x + 18.0F, details.y + 286.0F,
                    details.w - 36.0F, 5, 16.0F, SDL_Color{172, 180, 192, 255},
                    selectedCanSpend ? "This node can take a point right now."
                    : selectedRank >= selectedDefinition.maxRank
                        ? "This node is already maxed."
                    : runState.skillPoints <= 0
                        ? "Gain another level to earn a point."
                        : "Unlock the prerequisite branches first.");

    DrawButtonChrome(renderer_, spendButton,
                     selectedCanSpend ? SDL_Color{98, 140, 106, 255}
                                      : SDL_Color{68, 72, 78, 255},
                     SDL_Color{228, 228, 220, 255},
                     selectedCanSpend ? SDL_Color{166, 202, 164, 255}
                                      : SDL_Color{96, 96, 100, 255},
                     selectedCanSpend ? "Spend Point" : "Cannot Spend",
                     selectedCanSpend);
    DrawButtonChrome(renderer_, closeButton, SDL_Color{86, 92, 122, 255},
                     SDL_Color{236, 232, 224, 255},
                     SDL_Color{142, 150, 188, 255}, "Return to Run");
}

void FleshgolemGame::RenderRunEndOverlay() const {
    const SDL_FRect modal{382.0F, 182.0F, 516.0F, 386.0F};
    const SDL_FRect primaryButton = RunEndPrimaryButtonRect();
    const SDL_FRect secondaryButton = RunEndSecondaryButtonRect();

    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, kWindowWidth, kWindowHeight},
             SDL_Color{0, 0, 0, 176});
    DrawPanelChrome(renderer_, modal, SDL_Color{34, 26, 34, 245},
                    SDL_Color{206, 192, 180, 255},
                    sceneState_ == SceneState::Victory
                        ? SDL_Color{142, 198, 152, 255}
                        : SDL_Color{194, 108, 110, 255});

    DrawTextFit(renderer_, modal.x + 158.0F, modal.y + 30.0F, modal.w - 206.0F,
                SDL_Color{242, 232, 224, 255}, endTitle_);
    DrawWrappedText(renderer_, modal.x + 48.0F, modal.y + 70.0F,
                    modal.w - 96.0F, 2, 18.0F, SDL_Color{212, 204, 196, 255},
                    endSubtitle_);

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);

    std::ostringstream lineOne;
    lineOne << "Kills: " << resources.kills << "  Grafts: " << resources.grafts
            << "  Research: " << resources.research
            << "  Bosses: " << runState.bossesDefeated;
    DrawTextFit(renderer_, modal.x + 54.0F, modal.y + 126.0F, modal.w - 108.0F,
                SDL_Color{190, 196, 188, 255}, lineOne.str());

    std::ostringstream lineTwo;
    lineTwo << "Essence this run: " << resources.essence
            << "  Banked Essence: " << bankedEssence_
            << "  Last Payout: " << lastPayout_;
    DrawTextFit(renderer_, modal.x + 54.0F, modal.y + 150.0F, modal.w - 108.0F,
                SDL_Color{222, 204, 166, 255}, lineTwo.str());

    DrawWrappedText(renderer_, modal.x + 54.0F, modal.y + 206.0F,
                    modal.w - 108.0F, 3, 18.0F, SDL_Color{186, 182, 176, 255},
                    sceneState_ == SceneState::Victory
                        ? "The demo run is complete. Start another run or "
                          "return to the title "
                          "screen."
                        : "Retry immediately, or return to the title screen to "
                          "review your banked "
                          "Essence.");
    if (sceneState_ == SceneState::Defeat) {
        DrawWrappedText(
            renderer_, modal.x + 54.0F, modal.y + 260.0F, modal.w - 108.0F, 2,
            18.0F, SDL_Color{214, 142, 132, 255},
            "Press E at any time on this screen to dissolve the failed run.");
    }

    DrawButtonChrome(renderer_, primaryButton, SDL_Color{108, 136, 104, 255},
                     SDL_Color{236, 232, 224, 255},
                     SDL_Color{166, 202, 164, 255}, "Start New Run", true);

    DrawButtonChrome(renderer_, secondaryButton, SDL_Color{86, 92, 122, 255},
                     SDL_Color{236, 232, 224, 255},
                     SDL_Color{142, 150, 188, 255}, "Return to Title");
}
