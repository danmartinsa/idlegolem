#include <algorithm>
#include <numeric>
#include <vector>

#include "prototype_game.h"

using namespace prototype_game_internal;

namespace {

Attributes ScaleAttributes(const Attributes& source, const float factor) {
    Attributes scaled = source;
    scaled.might *= factor;
    scaled.vitality *= factor;
    scaled.agility *= factor;
    scaled.reason *= factor;
    scaled.instinct *= factor;
    scaled.corruption *= factor;
    return scaled;
}

void AddAttributes(Attributes* target, const Attributes& delta) {
    target->might += delta.might;
    target->vitality += delta.vitality;
    target->agility += delta.agility;
    target->reason += delta.reason;
    target->instinct += delta.instinct;
    target->corruption += delta.corruption;
}

}  // namespace

void PrototypeGame::SpawnPlayer() {
    player_ = registry_.create();

    const int marrowRanks = bankedEssence_ / 5;

    BaseAttributes base;
    base.might = 4.0F + static_cast<float>(marrowRanks / 2);
    base.vitality = 5.0F + static_cast<float>(marrowRanks / 2);
    base.agility = 3.0F + static_cast<float>(marrowRanks / 3);
    base.reason = 3.0F + static_cast<float>(marrowRanks / 3);
    base.instinct = 3.0F + static_cast<float>(marrowRanks / 3);

    Resources resources;
    resources.biomass = 12 + marrowRanks * 2;
    resources.boneMeal = 4 + marrowRanks;

    RunState runState;
    runState.encountersPerZone = ZoneDefinitionFor(0).encounters;
    runState.totalZones = static_cast<int>(kZoneCount);

    registry_.emplace<Name>(player_, "Prototype Homunculus");
    registry_.emplace<PlayerTag>(player_);
    registry_.emplace<BaseAttributes>(player_, base);
    registry_.emplace<Attributes>(player_);
    registry_.emplace<CombatStats>(player_);
    registry_.emplace<Health>(player_);
    registry_.emplace<CombatTimer>(player_, CombatTimer{0.4F});
    registry_.emplace<Body>(player_);
    registry_.emplace<Resources>(player_, resources);
    registry_.emplace<RunState>(player_, runState);
    registry_.emplace<StabilityState>(player_);
}

void PrototypeGame::BuildStarterBody() {
    auto createStarterPart =
        [this](const std::string& name, const SlotType slot, const Attributes bonus,
               const float healthBonus, const float attackBonus, const float attackIntervalBonus,
               const float critChanceBonus, const float armorBonus, const float evasionBonus,
               const float decayRate, const float stabilityDemand, const SDL_Color color) {
            const entt::entity part = registry_.create();
            BodyPart bodyPart;
            bodyPart.name = name;
            bodyPart.slot = slot;
            bodyPart.attributeBonus = bonus;
            bodyPart.healthBonus = healthBonus;
            bodyPart.attackBonus = attackBonus;
            bodyPart.attackIntervalBonus = attackIntervalBonus;
            bodyPart.critChanceBonus = critChanceBonus;
            bodyPart.armorBonus = armorBonus;
            bodyPart.evasionBonus = evasionBonus;
            bodyPart.freshness = 100.0F;
            bodyPart.decayRate = decayRate;
            bodyPart.stabilityDemand = stabilityDemand;
            bodyPart.color = color;

            registry_.emplace<BodyPart>(part, bodyPart);
            registry_.emplace<Name>(part, name);
            registry_.emplace<EquippedTo>(part, player_, slot);
            registry_.get<Body>(player_).slots[SlotIndex(slot)] = part;
        };

    createStarterPart("Stitched Skull", SlotType::Head,
                      Attributes{0.0F, 0.0F, 0.0F, 0.7F, 1.1F, 0.0F}, 0.0F, 0.0F, 0.0F, 0.02F, 0.0F,
                      0.01F, 0.06F, 0.8F, SDL_Color{105, 105, 124, 255});
    createStarterPart("Patchwork Torso", SlotType::Torso,
                      Attributes{0.0F, 2.2F, 0.0F, 0.0F, 0.0F, 0.0F}, 28.0F, 0.0F, 0.0F, 0.0F, 1.6F,
                      0.0F, 0.06F, 1.1F, SDL_Color{126, 88, 82, 255});
    createStarterPart("Scrap Left Arm", SlotType::LeftArm,
                      Attributes{1.2F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F}, 0.0F, 2.4F, -0.03F, 0.0F,
                      0.0F, 0.0F, 0.07F, 0.8F, SDL_Color{142, 112, 98, 255});
    createStarterPart("Scrap Right Arm", SlotType::RightArm,
                      Attributes{1.2F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F}, 0.0F, 2.4F, -0.03F, 0.0F,
                      0.0F, 0.0F, 0.07F, 0.8F, SDL_Color{142, 112, 98, 255});
    createStarterPart("Bent Left Leg", SlotType::LeftLeg,
                      Attributes{0.0F, 0.0F, 1.1F, 0.0F, 0.0F, 0.0F}, 0.0F, 0.0F, -0.02F, 0.0F,
                      0.0F, 0.015F, 0.08F, 0.8F, SDL_Color{108, 126, 100, 255});
    createStarterPart("Bent Right Leg", SlotType::RightLeg,
                      Attributes{0.0F, 0.0F, 1.1F, 0.0F, 0.0F, 0.0F}, 0.0F, 0.0F, -0.02F, 0.0F,
                      0.0F, 0.015F, 0.08F, 0.8F, SDL_Color{108, 126, 100, 255});
    createStarterPart("Donor Heart", SlotType::Heart,
                      Attributes{0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F}, 18.0F, 0.0F, 0.0F, 0.0F, 0.0F,
                      0.0F, 0.08F, 0.9F, SDL_Color{172, 72, 84, 255});
    createStarterPart("Threadbare Lungs", SlotType::Lungs,
                      Attributes{0.0F, 0.5F, 0.8F, 0.0F, 0.0F, 0.0F}, 0.0F, 0.0F, -0.02F, 0.0F,
                      0.0F, 0.01F, 0.07F, 0.8F, SDL_Color{132, 144, 160, 255});
    createStarterPart("Tallow Stomach", SlotType::Stomach,
                      Attributes{0.0F, 0.4F, 0.0F, 0.6F, 0.0F, 0.2F}, 6.0F, 0.0F, 0.0F, 0.0F, 0.0F,
                      0.0F, 0.06F, 0.9F, SDL_Color{176, 144, 108, 255});
    createStarterPart("Copper Spine", SlotType::Spine,
                      Attributes{0.8F, 0.0F, 0.0F, 0.8F, 0.0F, 0.0F}, 0.0F, 1.6F, 0.0F, 0.0F, 0.6F,
                      0.0F, 0.06F, 1.0F, SDL_Color{148, 118, 96, 255});
    createStarterPart("Hide Skin", SlotType::Skin, Attributes{0.0F, 0.6F, 0.0F, 0.0F, 0.0F, 0.0F},
                      8.0F, 0.0F, 0.0F, 0.0F, 1.8F, 0.005F, 0.05F, 0.8F,
                      SDL_Color{158, 124, 104, 255});
    createStarterPart("Hot Blood", SlotType::Blood, Attributes{0.0F, 0.0F, 0.0F, 0.0F, 0.7F, 0.4F},
                      0.0F, 0.0F, 0.0F, 0.01F, 0.0F, 0.0F, 0.07F, 0.9F,
                      SDL_Color{194, 88, 96, 255});
}

void PrototypeGame::SpawnEnemyIfNeeded(const float deltaSeconds) {
    if (sceneState_ != SceneState::Running || currentEnemy_ != entt::null ||
        currentCorpse_ != entt::null) {
        return;
    }

    enemySpawnDelay_ -= deltaSeconds;
    if (enemySpawnDelay_ > 0.0F) {
        return;
    }

    const RunState& runState = registry_.get<RunState>(player_);
    const bool boss = runState.zoneEncounter >= runState.encountersPerZone - 1;
    const bool elite = boss || runState.zoneEncounter == runState.encountersPerZone - 2 ||
                       runState.eliteCountdown <= 0;
    const int tier = 1 + runState.zoneIndex + runState.zoneEncounter / 2 + runState.depth / 5;
    SpawnEnemy(tier, elite, boss);
}

void PrototypeGame::SpawnEnemy(const int tier, const bool elite, const bool boss) {
    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);

    EnemyKind kind = zone.primary;
    if (boss) {
        kind = zone.boss;
    } else if (elite) {
        kind = zone.elite;
    } else {
        kind = RandomFloat(rng_, 0.0F, 1.0F) < 0.55F ? zone.primary : zone.secondary;
    }

    const EnemyTemplate enemyTemplate = EnemyTemplateFor(kind);
    const float scale = 1.0F + static_cast<float>(tier - 1) * 0.22F +
                        static_cast<float>(runState.zoneIndex) * 0.25F +
                        static_cast<float>(runState.zoneEncounter) * 0.06F +
                        (elite ? 0.35F : 0.0F) + (boss ? 0.7F : 0.0F);

    currentEnemy_ = registry_.create();
    std::string enemyName = enemyTemplate.name;
    if (boss) {
        enemyName = std::string("Bell-Tower ") + enemyTemplate.name;
    } else if (elite) {
        enemyName = std::string("Elite ") + enemyTemplate.name;
    }

    Enemy enemyInfo;
    enemyInfo.kind = kind;
    enemyInfo.tier = tier;
    enemyInfo.elite = elite;
    enemyInfo.boss = boss;

    CombatStats enemyStats;
    enemyStats.attackPower = enemyTemplate.attack * scale;
    enemyStats.attackInterval =
        Clamp(enemyTemplate.attackInterval - static_cast<float>(tier - 1) * 0.03F, 0.6F, 1.8F);
    enemyStats.critChance = boss ? 0.14F : (elite ? 0.1F : 0.05F);
    enemyStats.critMultiplier = boss ? 1.9F : 1.6F;
    enemyStats.armor = enemyTemplate.armor + static_cast<float>(tier - 1) * 0.6F +
                       (elite ? 1.0F : 0.0F) + (boss ? 1.8F : 0.0F);
    enemyStats.evasion = Clamp(enemyTemplate.evasion + (elite ? 0.04F : 0.0F), 0.0F, 0.32F);

    const float maxHealth = enemyTemplate.health * scale * (boss ? 1.35F : 1.0F);

    registry_.emplace<Name>(currentEnemy_, enemyName);
    registry_.emplace<EnemyTag>(currentEnemy_);
    registry_.emplace<Enemy>(currentEnemy_, enemyInfo);
    registry_.emplace<CombatStats>(currentEnemy_, enemyStats);
    registry_.emplace<Health>(currentEnemy_, Health{maxHealth, maxHealth});
    registry_.emplace<CombatTimer>(currentEnemy_, CombatTimer{0.4F});

    auto& mutableRunState = registry_.get<RunState>(player_);
    mutableRunState.encounters++;
    mutableRunState.highestTier = std::max(mutableRunState.highestTier, tier);

    enemyLaneX_ = kLaneEnemySpawnX;
    enemyApproachSpeed_ =
        116.0F + static_cast<float>(tier) * 10.0F + (elite ? 16.0F : 0.0F) + (boss ? 10.0F : 0.0F);
    combatJoined_ = false;

    std::ostringstream line;
    line << zone.name << " wave " << (mutableRunState.zoneEncounter + 1) << "/"
         << mutableRunState.encountersPerZone << ": " << enemyName << " enters the lane.";
    AddLog(line.str());
}

void PrototypeGame::UpdateAnimation(const float deltaSeconds) {
    playerAnimationTime_ += deltaSeconds;
    while (playerAnimationTime_ >= kWalkFrameDuration) {
        playerAnimationTime_ -= kWalkFrameDuration;
        playerAnimationFrame_ = (playerAnimationFrame_ + 1) % kPlayerFrameCount;
    }
}

void PrototypeGame::UpdateLane(const float deltaSeconds) {
    if (currentEnemy_ == entt::null || !registry_.valid(currentEnemy_)) {
        return;
    }

    if (combatJoined_) {
        enemyLaneX_ = kLaneEnemyContactX;
        return;
    }

    enemyLaneX_ -= enemyApproachSpeed_ * deltaSeconds;
    if (enemyLaneX_ > kLaneEnemyContactX) {
        return;
    }

    enemyLaneX_ = kLaneEnemyContactX;
    combatJoined_ = true;
    registry_.get<CombatTimer>(player_).attackCooldown = 0.12F;
    registry_.get<CombatTimer>(currentEnemy_).attackCooldown = 0.45F;
    AddLog(registry_.get<Name>(currentEnemy_).value + " reaches the graft line. Combat starts.");
}

void PrototypeGame::RecalculatePlayerStats() {
    if (player_ == entt::null || !registry_.valid(player_)) {
        return;
    }

    const BaseAttributes& base = registry_.get<BaseAttributes>(player_);
    Attributes current;
    current.might = base.might + runUpgrades_.attributeBonus.might;
    current.vitality = base.vitality + runUpgrades_.attributeBonus.vitality;
    current.agility = base.agility + runUpgrades_.attributeBonus.agility;
    current.reason = base.reason + runUpgrades_.attributeBonus.reason;
    current.instinct = base.instinct + runUpgrades_.attributeBonus.instinct;
    current.corruption = base.corruption + runUpgrades_.attributeBonus.corruption;

    float bonusHealth = runUpgrades_.healthBonus;
    float bonusAttack = runUpgrades_.attackBonus;
    float intervalBonus = runUpgrades_.attackIntervalBonus;
    float critBonus = runUpgrades_.critChanceBonus;
    float armorBonus = runUpgrades_.armorBonus;
    float evasionBonus = runUpgrades_.evasionBonus;
    float stabilityDemand = 0.0F;

    const Body& body = registry_.get<Body>(player_);
    for (const entt::entity partEntity : body.slots) {
        if (partEntity == entt::null || !registry_.valid(partEntity)) {
            continue;
        }

        const BodyPart& part = registry_.get<BodyPart>(partEntity);
        const float factor = Clamp(part.freshness / 100.0F, 0.15F, 1.0F);
        current.might += part.attributeBonus.might * factor;
        current.vitality += part.attributeBonus.vitality * factor;
        current.agility += part.attributeBonus.agility * factor;
        current.reason += part.attributeBonus.reason * factor;
        current.instinct += part.attributeBonus.instinct * factor;
        current.corruption += part.attributeBonus.corruption * factor;
        bonusHealth += part.healthBonus * factor;
        bonusAttack += part.attackBonus * factor;
        intervalBonus += part.attackIntervalBonus * factor;
        critBonus += part.critChanceBonus * factor;
        armorBonus += part.armorBonus * factor;
        evasionBonus += part.evasionBonus * factor;
        stabilityDemand += part.stabilityDemand;
    }

    StabilityState stability;
    stability.capacity = 7.0F + current.reason * 0.9F + current.vitality * 0.45F +
                         runUpgrades_.stabilityCapacityBonus;
    stability.demand = stabilityDemand;
    stability.overload = std::max(0.0F, stability.demand - stability.capacity);
    registry_.replace<StabilityState>(player_, stability);

    CombatStats stats;
    stats.attackPower =
        std::max(4.0F, 7.0F + current.might * 2.5F + bonusAttack - stability.overload * 1.2F);
    stats.attackInterval = Clamp(
        1.4F - current.agility * 0.05F + intervalBonus + stability.overload * 0.05F, 0.42F, 1.7F);
    stats.critChance = Clamp(
        0.04F + current.instinct * 0.01F + critBonus - stability.overload * 0.01F, 0.01F, 0.5F);
    stats.critMultiplier = 1.45F + current.reason * 0.04F;
    stats.armor =
        std::max(0.0F, current.vitality * 0.56F + armorBonus - stability.overload * 0.45F);
    stats.evasion =
        Clamp(current.agility * 0.012F + evasionBonus - stability.overload * 0.01F, 0.0F, 0.38F);

    Health& health = registry_.get<Health>(player_);
    const float previousMaximum = std::max(1.0F, health.maximum);
    const float missingHealth = std::max(0.0F, previousMaximum - health.current);
    health.maximum =
        std::max(36.0F, 78.0F + current.vitality * 16.0F + bonusHealth - stability.overload * 8.0F);
    health.current = Clamp(health.maximum - missingHealth, 0.0F, health.maximum);

    registry_.replace<Attributes>(player_, current);
    registry_.replace<CombatStats>(player_, stats);
}

void PrototypeGame::UpdateCombat(const float deltaSeconds) {
    if (!combatJoined_ || currentEnemy_ == entt::null || !registry_.valid(currentEnemy_)) {
        return;
    }

    CombatTimer& playerTimer = registry_.get<CombatTimer>(player_);
    CombatTimer& enemyTimer = registry_.get<CombatTimer>(currentEnemy_);
    const CombatStats& playerStats = registry_.get<CombatStats>(player_);
    const CombatStats& enemyStats = registry_.get<CombatStats>(currentEnemy_);

    playerTimer.attackCooldown -= deltaSeconds;
    enemyTimer.attackCooldown -= deltaSeconds;

    while (sceneState_ == SceneState::Running && currentEnemy_ != entt::null &&
           registry_.valid(currentEnemy_) && playerTimer.attackCooldown <= 0.0F) {
        ResolveAttack(player_, currentEnemy_);
        playerTimer.attackCooldown += playerStats.attackInterval;
    }

    while (sceneState_ == SceneState::Running && currentEnemy_ != entt::null &&
           registry_.valid(currentEnemy_) && enemyTimer.attackCooldown <= 0.0F) {
        ResolveAttack(currentEnemy_, player_);
        enemyTimer.attackCooldown += enemyStats.attackInterval;
    }
}

void PrototypeGame::ResolveAttack(const entt::entity attacker, const entt::entity defender) {
    if (!registry_.valid(attacker) || !registry_.valid(defender)) {
        return;
    }

    const Name& attackerName = registry_.get<Name>(attacker);
    const Name& defenderName = registry_.get<Name>(defender);
    const CombatStats& attackStats = registry_.get<CombatStats>(attacker);
    const CombatStats& defenseStats = registry_.get<CombatStats>(defender);

    if (RandomFloat(rng_, 0.0F, 1.0F) < defenseStats.evasion) {
        AddLog(defenderName.value + " evades " + attackerName.value + ".");
        return;
    }

    float damage = attackStats.attackPower * RandomFloat(rng_, 0.9F, 1.15F);
    const bool criticalHit = RandomFloat(rng_, 0.0F, 1.0F) < attackStats.critChance;
    if (criticalHit) {
        damage *= attackStats.critMultiplier;
    }

    damage = std::max(1.0F, damage - defenseStats.armor * 0.45F);
    Health& defenderHealth = registry_.get<Health>(defender);
    defenderHealth.current = std::max(0.0F, defenderHealth.current - damage);
    TriggerAttackAudio();

    std::ostringstream logLine;
    logLine << attackerName.value << " hits " << defenderName.value << " for "
            << static_cast<int>(std::round(damage));
    if (criticalHit) {
        logLine << " crit";
    }
    AddLog(logLine.str());

    if (defender != player_ && defenderHealth.current <= 0.0F) {
        OnEnemyDefeated(defender);
        return;
    }

    if (defender == player_ && defenderHealth.current <= 0.0F) {
        AddLog("The homunculus collapses into useless grafts.");
        FinishRun(false);
    }
}

void PrototypeGame::OnEnemyDefeated(const entt::entity enemy) {
    if (!registry_.valid(enemy)) {
        return;
    }

    const Enemy enemyInfo = registry_.get<Enemy>(enemy);
    const EnemyTemplate enemyTemplate = EnemyTemplateFor(enemyInfo.kind);
    Resources& resources = registry_.get<Resources>(player_);
    RunState& runState = registry_.get<RunState>(player_);

    resources.kills++;
    resources.essence += 1 + enemyInfo.tier + (enemyInfo.elite ? 2 : 0) + (enemyInfo.boss ? 2 : 0);
    runState.depth++;
    if (enemyInfo.elite) {
        runState.elitesDefeated++;
        runState.eliteCountdown = 3;
    } else {
        runState.eliteCountdown--;
    }
    if (enemyInfo.boss) {
        runState.bossesDefeated++;
    }

    corpseLaneX_ = enemyLaneX_;
    currentCorpse_ = registry_.create();

    Corpse corpse;
    corpse.source = enemyInfo.kind;
    corpse.tier = enemyInfo.tier;
    corpse.elite = enemyInfo.elite;
    corpse.boss = enemyInfo.boss;
    corpse.harvestTimer = enemyInfo.boss ? 1.4F : (enemyInfo.elite ? 1.1F : 0.8F);
    corpse.maxHarvestTimer = corpse.harvestTimer;
    corpse.biomassReward =
        RandomInt(rng_, 5 + enemyInfo.tier * 2, 8 + enemyInfo.tier * 3 + (enemyInfo.elite ? 4 : 0));
    corpse.boneMealReward =
        RandomInt(rng_, 2 + enemyInfo.tier, 4 + enemyInfo.tier * 2 + (enemyInfo.elite ? 2 : 0));
    corpse.researchReward = enemyInfo.boss ? 3 : (enemyInfo.elite ? 1 : 0);

    registry_.emplace<Name>(currentCorpse_, std::string("Remains of ") + enemyTemplate.name);
    registry_.emplace<Corpse>(currentCorpse_, corpse);

    combatJoined_ = false;
    AddLog(registry_.get<Name>(enemy).value + " falls. Harvest the remains.");
    registry_.destroy(enemy);
    currentEnemy_ = entt::null;
}

void PrototypeGame::UpdateHarvesting(const float deltaSeconds) {
    if (currentCorpse_ == entt::null || !registry_.valid(currentCorpse_)) {
        return;
    }

    Corpse& corpse = registry_.get<Corpse>(currentCorpse_);
    corpse.harvestTimer -= deltaSeconds;
    if (corpse.harvestTimer <= 0.0F) {
        HarvestCorpse(currentCorpse_);
    }
}

void PrototypeGame::HarvestCorpse(const entt::entity corpseEntity) {
    if (!registry_.valid(corpseEntity)) {
        return;
    }

    const Corpse corpse = registry_.get<Corpse>(corpseEntity);
    Resources& resources = registry_.get<Resources>(player_);
    RunState& runState = registry_.get<RunState>(player_);
    Body& body = registry_.get<Body>(player_);

    resources.biomass += static_cast<int>(
        std::round(static_cast<float>(corpse.biomassReward) * runUpgrades_.harvestMultiplier));
    resources.boneMeal += static_cast<int>(
        std::round(static_cast<float>(corpse.boneMealReward) * runUpgrades_.harvestMultiplier));
    resources.research += corpse.researchReward;

    const EnemyTemplate enemyTemplate = EnemyTemplateFor(corpse.source);
    const bool dropPart = corpse.boss || corpse.elite || RandomFloat(rng_, 0.0F, 1.0F) < 0.78F;

    if (dropPart) {
        const SlotType slot =
            enemyTemplate.lootSlots[RandomInt(rng_, 0, enemyTemplate.lootSlotCount - 1)];
        int rarity = std::min(5, corpse.tier - 1 + (corpse.elite ? 2 : 0) + (corpse.boss ? 1 : 0));
        rarity = std::max(0, rarity + (RandomFloat(rng_, 0.0F, 1.0F) < 0.22F ? 1 : 0));
        rarity = std::min(rarity, 5);
        const float scale =
            1.0F + static_cast<float>(corpse.tier - 1) * 0.3F + static_cast<float>(rarity) * 0.26F;

        Attributes bonus{};
        float healthBonus = 0.0F;
        float attackBonus = 0.0F;
        float intervalBonus = 0.0F;
        float critBonus = 0.0F;
        float armorBonus = 0.0F;
        float evasionBonus = 0.0F;

        switch (slot) {
            case SlotType::Head:
                bonus.reason = 0.6F * scale;
                bonus.instinct = 1.1F * scale;
                critBonus = 0.012F * scale;
                evasionBonus = 0.008F * scale;
                break;
            case SlotType::Torso:
                bonus.vitality = 1.4F * scale;
                healthBonus = 18.0F * scale;
                armorBonus = 1.2F * scale;
                break;
            case SlotType::LeftArm:
            case SlotType::RightArm:
                bonus.might = 1.15F * scale;
                attackBonus = 2.7F * scale;
                intervalBonus = -0.03F * scale;
                break;
            case SlotType::LeftLeg:
            case SlotType::RightLeg:
                bonus.agility = 1.1F * scale;
                intervalBonus = -0.02F * scale;
                evasionBonus = 0.012F * scale;
                break;
            case SlotType::Heart:
                bonus.vitality = 1.5F * scale;
                healthBonus = 24.0F * scale;
                break;
            case SlotType::Lungs:
                bonus.vitality = 0.8F * scale;
                bonus.agility = 0.7F * scale;
                evasionBonus = 0.01F * scale;
                break;
            case SlotType::Stomach:
                bonus.reason = 0.9F * scale;
                bonus.corruption = 0.3F * scale;
                break;
            case SlotType::Spine:
                bonus.might = 0.9F * scale;
                bonus.reason = 0.9F * scale;
                attackBonus = 1.7F * scale;
                break;
            case SlotType::Skin:
                bonus.vitality = 0.8F * scale;
                armorBonus = 2.0F * scale;
                evasionBonus = 0.005F * scale;
                break;
            case SlotType::Blood:
                bonus.instinct = 0.5F * scale;
                bonus.corruption = 0.5F * scale;
                critBonus = 0.01F * scale;
                break;
        }

        const entt::entity part = registry_.create();
        const std::string partName = BuildPartName(corpse.source, slot);
        BodyPart bodyPart;
        bodyPart.name = partName;
        bodyPart.slot = slot;
        bodyPart.rarity = rarity;
        bodyPart.attributeBonus = bonus;
        bodyPart.healthBonus = healthBonus;
        bodyPart.attackBonus = attackBonus;
        bodyPart.attackIntervalBonus = intervalBonus;
        bodyPart.critChanceBonus = critBonus;
        bodyPart.armorBonus = armorBonus;
        bodyPart.evasionBonus = evasionBonus;
        bodyPart.freshness = 100.0F;
        bodyPart.decayRate =
            0.07F + static_cast<float>(corpse.tier) * 0.015F + static_cast<float>(rarity) * 0.01F;
        bodyPart.stabilityDemand = 0.8F + scale * 0.55F;
        bodyPart.color = RarityColor(rarity);

        registry_.emplace<Name>(part, partName);
        registry_.emplace<BodyPart>(part, bodyPart);

        const auto evaluatePartScore = [this](const entt::entity entity) {
            const BodyPart& part = registry_.get<BodyPart>(entity);
            const float freshnessFactor = Clamp(part.freshness / 100.0F, 0.15F, 1.0F);
            return freshnessFactor *
                   (part.attackBonus * 4.5F + part.healthBonus * 0.32F +
                    part.attributeBonus.might * 3.0F + part.attributeBonus.vitality * 2.4F +
                    part.attributeBonus.agility * 2.2F + part.attributeBonus.reason * 1.6F +
                    part.attributeBonus.instinct * 2.0F + part.critChanceBonus * 110.0F +
                    part.armorBonus * 2.2F + part.evasionBonus * 120.0F -
                    part.stabilityDemand * 0.4F);
        };

        const std::size_t slotIndex = SlotIndex(slot);
        const entt::entity equipped = body.slots[slotIndex];
        const float newScore = evaluatePartScore(part);
        const float oldScore = (equipped != entt::null && registry_.valid(equipped))
                                   ? evaluatePartScore(equipped)
                                   : -1.0F;

        if (equipped == entt::null || newScore > oldScore * 1.08F) {
            if (equipped != entt::null && registry_.valid(equipped)) {
                resources.biomass += 2 + registry_.get<BodyPart>(equipped).rarity * 2;
                resources.salvaged++;
                AddLog("Auto-salvaged replaced " + registry_.get<Name>(equipped).value + ".");
                registry_.destroy(equipped);
            }

            body.slots[slotIndex] = part;
            registry_.emplace<EquippedTo>(part, player_, slot);
            resources.grafts++;

            std::ostringstream line;
            line << "Grafted " << RarityName(rarity) << " " << partName << ".";
            AddLog(line.str());
        } else {
            resources.biomass += 2 + rarity * 2;
            resources.salvaged++;
            AddLog("Salvaged spare " + partName + " for Biomass.");
            registry_.destroy(part);
        }
    } else {
        AddLog("The remains yield only slurry, Bone Meal, and research scraps.");
    }

    runState.totalClears++;
    runState.zoneEncounter++;

    const bool zoneCleared = runState.zoneEncounter >= runState.encountersPerZone;

    registry_.destroy(corpseEntity);
    currentCorpse_ = entt::null;

    if (corpse.boss || zoneCleared) {
        pendingZoneAdvance_ = zoneCleared;
        pendingVictory_ = zoneCleared && runState.zoneIndex + 1 >= runState.totalZones;
        OpenRewardDraft(true, zoneCleared);
        return;
    }

    if (corpse.elite) {
        OpenRewardDraft(false, false);
        return;
    }

    enemySpawnDelay_ = 1.0F;
}

void PrototypeGame::UpdateDecay(const float deltaSeconds) {
    const float overload = registry_.get<StabilityState>(player_).overload;
    const float multiplier = (1.0F + overload * 0.22F) * runUpgrades_.decayMultiplier;

    auto view = registry_.view<BodyPart, EquippedTo>();
    for (auto [entity, part, equipped] : view.each()) {
        if (equipped.owner != player_) {
            continue;
        }

        const float previousFreshness = part.freshness;
        part.freshness =
            std::max(0.0F, part.freshness - part.decayRate * multiplier * deltaSeconds);
        if (previousFreshness > 0.0F && part.freshness == 0.0F) {
            AddLog(part.name + " has fully decayed.");
        }
    }
}

void PrototypeGame::AdvanceToNextZone() {
    RunState& runState = registry_.get<RunState>(player_);
    runState.zoneIndex = std::min(runState.zoneIndex + 1, runState.totalZones - 1);
    runState.zoneEncounter = 0;
    runState.encountersPerZone = ZoneDefinitionFor(runState.zoneIndex).encounters;
    runState.eliteCountdown = 3;

    Health& health = registry_.get<Health>(player_);
    health.current = std::min(health.maximum, health.current + health.maximum * 0.35F);

    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);
    AddLog(std::string("Advancing into ") + zone.name + ".");
    AddLog(zone.subtitle);
    enemySpawnDelay_ = 1.2F;
}

void PrototypeGame::OpenRewardDraft(const bool majorReward, const bool zoneClearReward) {
    rewardOptions_.fill(RewardOption{});
    rewardSelectionIndex_ = 0;
    sceneState_ = SceneState::RewardDraft;
    paused_ = false;

    const RunState& runState = registry_.get<RunState>(player_);
    const ZoneDefinition& zone = ZoneDefinitionFor(runState.zoneIndex);
    const float scale = majorReward ? 1.7F : 1.0F;

    rewardDraftTitle_ =
        zoneClearReward ? std::string("Zone Cleared: ") + zone.name : std::string("Reward Draft");
    rewardDraftSubtitle_ = zoneClearReward ? "Choose one graft protocol before the next march."
                                           : "Choose one elite trophy to steer this run.";

    std::vector<RewardOption> pool;
    pool.reserve(8);

    RewardOption marrowSurge;
    marrowSurge.name = "Marrow Surge";
    marrowSurge.description = "More Might, Agility, attack power, and a faster rhythm.";
    marrowSurge.color = SDL_Color{178, 92, 86, 255};
    marrowSurge.attributeBonus =
        ScaleAttributes(Attributes{1.4F, 0.0F, 0.9F, 0.0F, 0.0F, 0.0F}, scale);
    marrowSurge.attackBonus = 2.4F * scale;
    marrowSurge.attackIntervalBonus = -0.04F * scale;
    pool.push_back(marrowSurge);

    RewardOption ironSutures;
    ironSutures.name = "Iron Sutures";
    ironSutures.description = "More Vitality, armor, and stability capacity.";
    ironSutures.color = SDL_Color{128, 148, 164, 255};
    ironSutures.attributeBonus =
        ScaleAttributes(Attributes{0.0F, 1.6F, 0.0F, 0.0F, 0.0F, 0.0F}, scale);
    ironSutures.healthBonus = 18.0F * scale;
    ironSutures.armorBonus = 1.6F * scale;
    ironSutures.stabilityCapacityBonus = 1.2F * scale;
    pool.push_back(ironSutures);

    RewardOption predatorNerves;
    predatorNerves.name = "Predator Nerves";
    predatorNerves.description = "More Instinct, crit, evasion, and cleaner strikes.";
    predatorNerves.color = SDL_Color{112, 162, 112, 255};
    predatorNerves.attributeBonus =
        ScaleAttributes(Attributes{0.0F, 0.0F, 0.0F, 0.0F, 1.5F, 0.0F}, scale);
    predatorNerves.critChanceBonus = 0.018F * scale;
    predatorNerves.evasionBonus = 0.018F * scale;
    pool.push_back(predatorNerves);

    RewardOption neuralLattice;
    neuralLattice.name = "Neural Lattice";
    neuralLattice.description = "More Reason, better harvests, and broader stability.";
    neuralLattice.color = SDL_Color{106, 132, 208, 255};
    neuralLattice.attributeBonus =
        ScaleAttributes(Attributes{0.0F, 0.0F, 0.0F, 1.4F, 0.0F, 0.0F}, scale);
    neuralLattice.harvestMultiplierFactor = 1.0F + 0.16F * scale;
    neuralLattice.stabilityCapacityBonus = 0.8F * scale;
    neuralLattice.researchBonus = static_cast<int>(std::round(1.0F * scale));
    pool.push_back(neuralLattice);

    RewardOption preservationBath;
    preservationBath.name = "Preservation Bath";
    preservationBath.description = "Slower decay, a strong heal, and more biomass stock.";
    preservationBath.color = SDL_Color{96, 180, 180, 255};
    preservationBath.decayMultiplierFactor = 0.86F - (majorReward ? 0.08F : 0.0F);
    preservationBath.immediateHealPercent = 0.32F + (majorReward ? 0.16F : 0.0F);
    preservationBath.biomassBonus = 10 + (majorReward ? 12 : 0);
    pool.push_back(preservationBath);

    RewardOption blackIchor;
    blackIchor.name = "Black Ichor";
    blackIchor.description = "Aggressive corrupted power with bonus essence and crit.";
    blackIchor.color = SDL_Color{164, 82, 166, 255};
    blackIchor.attributeBonus =
        ScaleAttributes(Attributes{0.6F, 0.0F, 0.0F, 0.0F, 0.6F, 1.2F}, scale);
    blackIchor.attackBonus = 2.0F * scale;
    blackIchor.critChanceBonus = 0.012F * scale;
    blackIchor.essenceBonus = static_cast<int>(std::round(2.0F * scale));
    pool.push_back(blackIchor);

    RewardOption stitcherCache;
    stitcherCache.name = "Stitcher Cache";
    stitcherCache.description = "Immediate Biomass, Bone Meal, Research, and a partial heal.";
    stitcherCache.color = SDL_Color{200, 168, 94, 255};
    stitcherCache.biomassBonus = 12 + (majorReward ? 10 : 0);
    stitcherCache.boneMealBonus = 8 + (majorReward ? 6 : 0);
    stitcherCache.researchBonus = 1 + (majorReward ? 2 : 0);
    stitcherCache.immediateHealPercent = 0.2F + (majorReward ? 0.1F : 0.0F);
    pool.push_back(stitcherCache);

    RewardOption warSpine;
    warSpine.name = "War Spine";
    warSpine.description = "More Might, Reason, armor, and direct attack force.";
    warSpine.color = SDL_Color{170, 116, 84, 255};
    warSpine.attributeBonus =
        ScaleAttributes(Attributes{1.0F, 0.0F, 0.0F, 0.9F, 0.0F, 0.0F}, scale);
    warSpine.attackBonus = 1.8F * scale;
    warSpine.armorBonus = 1.2F * scale;
    pool.push_back(warSpine);

    std::vector<int> indices(pool.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng_);

    for (std::size_t index = 0; index < kRewardChoiceCount; ++index) {
        rewardOptions_[index] = pool[static_cast<std::size_t>(indices[index])];
    }

    registry_.get<RunState>(player_).rewardDrafts++;
    TriggerRewardAudio();
    AddLog("The stitching table offers three graft protocols.");
}

void PrototypeGame::ApplyRewardChoice(const int index) {
    if (index < 0 || index >= static_cast<int>(kRewardChoiceCount)) {
        return;
    }

    RewardOption option = rewardOptions_[static_cast<std::size_t>(index)];
    rewardSelectionIndex_ = index;

    AddAttributes(&runUpgrades_.attributeBonus, option.attributeBonus);
    runUpgrades_.healthBonus += option.healthBonus;
    runUpgrades_.attackBonus += option.attackBonus;
    runUpgrades_.attackIntervalBonus += option.attackIntervalBonus;
    runUpgrades_.critChanceBonus += option.critChanceBonus;
    runUpgrades_.armorBonus += option.armorBonus;
    runUpgrades_.evasionBonus += option.evasionBonus;
    runUpgrades_.stabilityCapacityBonus += option.stabilityCapacityBonus;
    runUpgrades_.decayMultiplier *= option.decayMultiplierFactor;
    runUpgrades_.harvestMultiplier *= option.harvestMultiplierFactor;
    runUpgrades_.rewardHealPercent =
        std::max(runUpgrades_.rewardHealPercent, option.immediateHealPercent * 0.4F);

    Resources& resources = registry_.get<Resources>(player_);
    resources.biomass += option.biomassBonus;
    resources.boneMeal += option.boneMealBonus;
    resources.essence += option.essenceBonus;
    resources.research += option.researchBonus;

    RecalculatePlayerStats();
    if (option.immediateHealPercent > 0.0F) {
        Health& health = registry_.get<Health>(player_);
        health.current =
            std::min(health.maximum, health.current + health.maximum * option.immediateHealPercent);
    }

    AddLog("Selected reward: " + option.name + ".");

    if (pendingVictory_) {
        pendingVictory_ = false;
        pendingZoneAdvance_ = false;
        FinishRun(true);
        return;
    }

    if (pendingZoneAdvance_) {
        pendingZoneAdvance_ = false;
        AdvanceToNextZone();
    } else {
        enemySpawnDelay_ = 1.0F;
    }

    sceneState_ = SceneState::Running;
}

void PrototypeGame::FinishRun(const bool victory) {
    if (player_ == entt::null || !registry_.valid(player_)) {
        return;
    }

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);
    const int payout =
        std::max(2, resources.essence + resources.research * 2 + resources.kills / 2 +
                        runState.bossesDefeated * 4 + runState.zoneIndex * 3);

    lastPayout_ = payout;
    paused_ = false;
    combatJoined_ = false;
    currentEnemy_ = entt::null;
    currentCorpse_ = entt::null;

    if (victory) {
        bankedEssence_ += payout;
        completedRuns_++;
        sceneState_ = SceneState::Victory;
        endTitle_ = "Demo complete";
        endSubtitle_ = "The final frame holds. Essence banked: +" + std::to_string(payout) + ".";
        TriggerVictoryAudio();
        AddLog("Demo victory. Primordial Essence banked.");
    } else {
        sceneState_ = SceneState::Defeat;
        endTitle_ = "Run failed";
        endSubtitle_ = "Press E to dissolve the remains for +" + std::to_string(payout) +
                       " Essence, or R to retry.";
        TriggerFailureAudio();
    }
}

void PrototypeGame::DissolveRun() {
    if (player_ == entt::null || !registry_.valid(player_)) {
        return;
    }

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);
    const int payout = std::max(1, resources.essence + resources.kills / 2 + resources.grafts / 2 +
                                       resources.boneMeal / 5 + resources.research +
                                       runState.highestTier + runState.bossesDefeated * 2);

    bankedEssence_ += payout;
    completedRuns_++;
    lastPayout_ = payout;
    ResetRun();
    sceneState_ = SceneState::Title;

    std::ostringstream line;
    line << "Dissolve complete: +" << payout << " Primordial Essence banked.";
    AddLog(line.str());
}
