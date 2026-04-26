#pragma once

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <entt/entt.hpp>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace fleshgolem_internal {

constexpr std::size_t kSlotCount = 12;
constexpr std::size_t kEnemyTypeCount = 4;
constexpr std::size_t kZoneCount = 3;
constexpr std::size_t kSkillNodeCount = 8;
constexpr float kMaxDeltaSeconds = 0.05F;
constexpr int kPlayerFrameCount = 4;
constexpr float kWalkFrameDuration = 0.12F;
constexpr float kLanePlayerX = 92.0F;
constexpr float kLaneEnemySpawnX = 548.0F;
constexpr float kLaneEnemyContactX = 238.0F;
constexpr float kLaneGroundOffsetY = 172.0F;
constexpr float kPlayerRenderWidth = 144.0F;
constexpr float kPlayerRenderHeight = 144.0F;
constexpr float kEnemyRenderWidth = 126.0F;
constexpr float kEnemyRenderHeight = 132.0F;
constexpr float kWindowWidth = 1280.0F;
constexpr float kWindowHeight = 720.0F;
constexpr int kAudioSampleRate = 48000;
constexpr float kPi = 3.1415926535F;

enum class SlotType : std::uint8_t {
    Head,
    Torso,
    LeftArm,
    RightArm,
    LeftLeg,
    RightLeg,
    Heart,
    Lungs,
    Stomach,
    Spine,
    Skin,
    Blood,
};

enum class EnemyKind : std::uint8_t {
    Scavenger,
    Wolf,
    Cultist,
    FailedHomunculus,
};

enum class SceneState : std::uint8_t {
    Title,
    Running,
    SkillTree,
    Victory,
    Defeat,
};

struct Name {
    std::string value;
};

struct PlayerTag {};

struct EnemyTag {};

struct BaseAttributes {
    float might = 0.0F;
    float vitality = 0.0F;
    float agility = 0.0F;
    float reason = 0.0F;
    float instinct = 0.0F;
    float corruption = 0.0F;
};

struct Attributes {
    float might = 0.0F;
    float vitality = 0.0F;
    float agility = 0.0F;
    float reason = 0.0F;
    float instinct = 0.0F;
    float corruption = 0.0F;
};

struct CombatStats {
    float attackPower = 0.0F;
    float attackInterval = 1.0F;
    float critChance = 0.0F;
    float critMultiplier = 1.5F;
    float armor = 0.0F;
    float evasion = 0.0F;
};

struct Health {
    float current = 0.0F;
    float maximum = 0.0F;
};

struct CombatTimer {
    float attackCooldown = 0.0F;
};

struct Body {
    std::array<entt::entity, kSlotCount> slots{};

    Body() { slots.fill(entt::null); }
};

struct Resources {
    int biomass = 0;
    int boneMeal = 0;
    int essence = 0;
    int research = 0;
    int kills = 0;
    int grafts = 0;
    int salvaged = 0;
};

struct RunState {
    int depth = 1;
    int encounters = 0;
    int totalClears = 0;
    int eliteCountdown = 3;
    int highestTier = 1;
    int level = 1;
    int experience = 0;
    int experienceToNextLevel = 9;
    int skillPoints = 0;
    int spentSkillPoints = 0;
    int zoneIndex = 0;
    int zoneEncounter = 0;
    int encountersPerZone = 5;
    int bossesDefeated = 0;
    int elitesDefeated = 0;
    int totalZones = static_cast<int>(kZoneCount);
    std::array<int, kSkillNodeCount> skillRanks{};
};

struct StabilityState {
    float demand = 0.0F;
    float capacity = 0.0F;
    float overload = 0.0F;
};

struct BodyPart {
    std::string name;
    SlotType slot = SlotType::Head;
    int rarity = 0;
    Attributes attributeBonus{};
    float healthBonus = 0.0F;
    float attackBonus = 0.0F;
    float attackIntervalBonus = 0.0F;
    float critChanceBonus = 0.0F;
    float armorBonus = 0.0F;
    float evasionBonus = 0.0F;
    float freshness = 100.0F;
    float decayRate = 0.1F;
    float stabilityDemand = 0.0F;
    SDL_Color color{160, 160, 160, 255};
};

struct EquippedTo {
    entt::entity owner = entt::null;
    SlotType slot = SlotType::Head;
};

struct Enemy {
    EnemyKind kind = EnemyKind::Scavenger;
    int tier = 1;
    bool elite = false;
    bool boss = false;
};

struct Corpse {
    EnemyKind source = EnemyKind::Scavenger;
    int tier = 1;
    bool elite = false;
    bool boss = false;
    float harvestTimer = 1.0F;
    float maxHarvestTimer = 1.0F;
    int biomassReward = 0;
    int boneMealReward = 0;
    int researchReward = 0;
};

struct EnemyTemplate {
    EnemyKind kind;
    const char* name;
    SDL_Color color;
    float health;
    float attack;
    float attackInterval;
    float armor;
    float evasion;
    std::array<SlotType, 4> lootSlots;
    int lootSlotCount;
};

struct ZoneDefinition {
    const char* name;
    const char* subtitle;
    SDL_Color skyTop;
    SDL_Color skyBottom;
    SDL_Color ground;
    SDL_Color accent;
    EnemyKind primary;
    EnemyKind secondary;
    EnemyKind elite;
    EnemyKind boss;
    int encounters;
};

struct RunUpgrades {
    Attributes attributeBonus{};
    float healthBonus = 0.0F;
    float attackBonus = 0.0F;
    float attackIntervalBonus = 0.0F;
    float critChanceBonus = 0.0F;
    float armorBonus = 0.0F;
    float evasionBonus = 0.0F;
    float stabilityCapacityBonus = 0.0F;
    float decayMultiplier = 1.0F;
    float harvestMultiplier = 1.0F;
    float harvestSpeedMultiplier = 1.0F;
    float levelHealPercent = 0.0F;
    int bonusEssencePerKill = 0;
    int researchOnEliteKill = 0;
};

struct SkillEffect {
    Attributes attributeBonus{};
    float healthBonus = 0.0F;
    float attackBonus = 0.0F;
    float attackIntervalBonus = 0.0F;
    float critChanceBonus = 0.0F;
    float armorBonus = 0.0F;
    float evasionBonus = 0.0F;
    float stabilityCapacityBonus = 0.0F;
    float decayMultiplierBonus = 0.0F;
    float harvestMultiplierBonus = 0.0F;
    float harvestSpeedMultiplierBonus = 0.0F;
    float levelHealPercentBonus = 0.0F;
    int bonusEssencePerKill = 0;
    int researchOnEliteKill = 0;
};

struct SkillNodeDefinition {
    const char* name = "";
    const char* shortName = "";
    const char* description = "";
    SDL_Color color{120, 120, 120, 255};
    SDL_FPoint position{0.0F, 0.0F};
    int maxRank = 1;
    std::array<int, 2> prerequisites{-1, -1};
    int prerequisiteCount = 0;
    SkillEffect effectPerRank{};
};

constexpr std::array<SlotType, kSlotCount> kAllSlots{
    SlotType::Head,    SlotType::Torso,    SlotType::LeftArm, SlotType::RightArm,
    SlotType::LeftLeg, SlotType::RightLeg, SlotType::Heart,   SlotType::Lungs,
    SlotType::Stomach, SlotType::Spine,    SlotType::Skin,    SlotType::Blood,
};

constexpr std::array<ZoneDefinition, kZoneCount> kZoneDefinitions{{
    ZoneDefinition{
        "Charnel Pits",
        "Scavenge the butcher trenches and stabilize the first body.",
        SDL_Color{44, 20, 26, 255},
        SDL_Color{94, 42, 48, 255},
        SDL_Color{86, 54, 48, 255},
        SDL_Color{196, 132, 90, 255},
        EnemyKind::Scavenger,
        EnemyKind::Wolf,
        EnemyKind::Cultist,
        EnemyKind::FailedHomunculus,
        5,
    },
    ZoneDefinition{
        "Ash Chapel",
        "Cross the occult nave where zealots stitch themselves into saints.",
        SDL_Color{26, 24, 44, 255},
        SDL_Color{82, 52, 104, 255},
        SDL_Color{62, 58, 70, 255},
        SDL_Color{212, 174, 104, 255},
        EnemyKind::Cultist,
        EnemyKind::Scavenger,
        EnemyKind::FailedHomunculus,
        EnemyKind::Cultist,
        5,
    },
    ZoneDefinition{
        "Black Cathedral",
        "March beneath iron bells and claim a final perfected frame.",
        SDL_Color{16, 18, 28, 255},
        SDL_Color{44, 66, 90, 255},
        SDL_Color{54, 58, 74, 255},
        SDL_Color{150, 210, 216, 255},
        EnemyKind::FailedHomunculus,
        EnemyKind::Cultist,
        EnemyKind::Wolf,
        EnemyKind::FailedHomunculus,
        6,
    },
}};

inline const std::array<SkillNodeDefinition, kSkillNodeCount> kSkillTreeDefinitions{{
    SkillNodeDefinition{
        "Bone Lattice",
        "BL",
        "Densify the frame with better marrow braces. Each rank adds Vitality, health, and armor.",
        SDL_Color{134, 144, 170, 255},
        SDL_FPoint{40.0F, 54.0F},
        3,
        {-1, -1},
        0,
        SkillEffect{.attributeBonus = Attributes{0.0F, 1.2F, 0.0F, 0.0F, 0.0F, 0.0F},
                    .healthBonus = 12.0F,
                    .armorBonus = 0.6F},
    },
    SkillNodeDefinition{
        "Sinew Knot",
        "SK",
        "Reinforce armature and tendon pulls. Each rank adds Might and direct attack power.",
        SDL_Color{176, 108, 96, 255},
        SDL_FPoint{40.0F, 182.0F},
        3,
        {-1, -1},
        0,
        SkillEffect{.attributeBonus = Attributes{1.1F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F},
                    .attackBonus = 2.2F},
    },
    SkillNodeDefinition{
        "Reflex Loop",
        "RL",
        "Tune the gait and snap of the body. Each rank grants Agility, speed, and evasion.",
        SDL_Color{110, 154, 118, 255},
        SDL_FPoint{40.0F, 310.0F},
        3,
        {-1, -1},
        0,
        SkillEffect{.attributeBonus = Attributes{0.0F, 0.0F, 0.9F, 0.0F, 0.0F, 0.0F},
                    .attackIntervalBonus = -0.03F,
                    .evasionBonus = 0.01F},
    },
    SkillNodeDefinition{
        "Occult Matrix",
        "OM",
        "Open more stable neural channels. Each rank adds Reason, stability, and level-up healing.",
        SDL_Color{116, 132, 202, 255},
        SDL_FPoint{276.0F, 54.0F},
        2,
        {0, -1},
        1,
        SkillEffect{.attributeBonus = Attributes{0.0F, 0.0F, 0.0F, 1.1F, 0.0F, 0.0F},
                    .stabilityCapacityBonus = 1.2F,
                    .levelHealPercentBonus = 0.05F},
    },
    SkillNodeDefinition{
        "Predator Heart",
        "PH",
        "Teach the chassis to hunt. Each rank adds Instinct, crit, and extra essence on kills.",
        SDL_Color{120, 170, 112, 255},
        SDL_FPoint{276.0F, 182.0F},
        2,
        {1, 2},
        2,
        SkillEffect{.attributeBonus = Attributes{0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F},
                    .critChanceBonus = 0.018F,
                    .bonusEssencePerKill = 1},
    },
    SkillNodeDefinition{
        "Preservation Bath",
        "PB",
        "Suspend rot while the organs acclimate. Each rank slows decay and speeds harvesting.",
        SDL_Color{96, 176, 182, 255},
        SDL_FPoint{276.0F, 310.0F},
        2,
        {0, 3},
        2,
        SkillEffect{.healthBonus = 8.0F,
                    .decayMultiplierBonus = -0.12F,
                    .harvestSpeedMultiplierBonus = 0.16F},
    },
    SkillNodeDefinition{
        "Grave Refinery",
        "GR",
        "Refine every carcass into usable stock. Each rank boosts harvest yield and elite "
        "research.",
        SDL_Color{192, 166, 92, 255},
        SDL_FPoint{512.0F, 118.0F},
        2,
        {3, 5},
        2,
        SkillEffect{.harvestMultiplierBonus = 0.18F, .researchOnEliteKill = 1},
    },
    SkillNodeDefinition{
        "Cathedral Frame",
        "CF",
        "Complete the combat chassis. Unlocking it boosts the entire body and hit output at once.",
        SDL_Color{176, 94, 164, 255},
        SDL_FPoint{512.0F, 246.0F},
        1,
        {4, 6},
        2,
        SkillEffect{.attributeBonus = Attributes{0.8F, 0.8F, 0.0F, 0.8F, 0.0F, 0.4F},
                    .healthBonus = 18.0F,
                    .attackBonus = 4.0F,
                    .critChanceBonus = 0.02F,
                    .stabilityCapacityBonus = 1.6F,
                    .bonusEssencePerKill = 2},
    },
}};

constexpr EnemyTemplate EnemyTemplateFor(const EnemyKind kind) {
    switch (kind) {
        case EnemyKind::Scavenger:
            return {
                kind,
                "Grave Scavenger",
                SDL_Color{164, 108, 96, 255},
                42.0F,
                8.0F,
                1.45F,
                1.0F,
                0.05F,
                {SlotType::LeftArm, SlotType::RightArm, SlotType::Head, SlotType::Torso},
                4,
            };
        case EnemyKind::Wolf:
            return {
                kind,
                "Carrion Wolf",
                SDL_Color{105, 138, 112, 255},
                36.0F,
                9.0F,
                1.18F,
                0.5F,
                0.12F,
                {SlotType::LeftLeg, SlotType::RightLeg, SlotType::Skin, SlotType::Head},
                4,
            };
        case EnemyKind::Cultist:
            return {
                kind,
                "Ash Chapel Cultist",
                SDL_Color{122, 92, 154, 255},
                48.0F,
                10.0F,
                1.32F,
                1.8F,
                0.08F,
                {SlotType::Head, SlotType::Lungs, SlotType::Blood, SlotType::Heart},
                4,
            };
        case EnemyKind::FailedHomunculus:
            return {
                kind,
                "Failed Homunculus",
                SDL_Color{180, 72, 88, 255},
                62.0F,
                11.0F,
                1.36F,
                2.8F,
                0.04F,
                {SlotType::Heart, SlotType::Spine, SlotType::Torso, SlotType::LeftArm},
                4,
            };
    }

    return EnemyTemplateFor(EnemyKind::Scavenger);
}

inline const ZoneDefinition& ZoneDefinitionFor(const int zoneIndex) {
    return kZoneDefinitions[std::clamp(zoneIndex, 0, static_cast<int>(kZoneCount) - 1)];
}

inline const SkillNodeDefinition& SkillDefinition(const int skillIndex) {
    return kSkillTreeDefinitions[std::clamp(skillIndex, 0, static_cast<int>(kSkillNodeCount) - 1)];
}

inline bool SkillPrerequisitesMet(const SkillNodeDefinition& definition,
                                  const std::array<int, kSkillNodeCount>& ranks) {
    for (int index = 0; index < definition.prerequisiteCount; ++index) {
        const int prerequisite = definition.prerequisites[static_cast<std::size_t>(index)];
        if (prerequisite < 0 || prerequisite >= static_cast<int>(kSkillNodeCount)) {
            return false;
        }
        if (ranks[static_cast<std::size_t>(prerequisite)] <= 0) {
            return false;
        }
    }
    return true;
}

constexpr std::size_t EnemyKindIndex(const EnemyKind kind) {
    return static_cast<std::size_t>(kind);
}

constexpr const char* EnemySpriteFileName(const EnemyKind kind) {
    switch (kind) {
        case EnemyKind::Scavenger:
            return "enemy_scavenger_walk.bmp";
        case EnemyKind::Wolf:
            return "enemy_wolf_walk.bmp";
        case EnemyKind::Cultist:
            return "enemy_cultist_walk.bmp";
        case EnemyKind::FailedHomunculus:
            return "enemy_failed_homunculus_walk.bmp";
    }

    return "enemy_scavenger_walk.bmp";
}

inline const char* ZoneBackdropFileName(const int zoneIndex) {
    switch (std::clamp(zoneIndex, 0, static_cast<int>(kZoneCount) - 1)) {
        case 0:
            return "backdrop_charnel_pits.bmp";
        case 1:
            return "backdrop_ash_chapel.bmp";
        default:
            return "backdrop_black_cathedral.bmp";
    }
}

constexpr std::size_t SlotIndex(const SlotType slot) { return static_cast<std::size_t>(slot); }

constexpr const char* SlotLabel(const SlotType slot) {
    switch (slot) {
        case SlotType::Head:
            return "Head";
        case SlotType::Torso:
            return "Torso";
        case SlotType::LeftArm:
            return "Left Arm";
        case SlotType::RightArm:
            return "Right Arm";
        case SlotType::LeftLeg:
            return "Left Leg";
        case SlotType::RightLeg:
            return "Right Leg";
        case SlotType::Heart:
            return "Heart";
        case SlotType::Lungs:
            return "Lungs";
        case SlotType::Stomach:
            return "Stomach";
        case SlotType::Spine:
            return "Spine";
        case SlotType::Skin:
            return "Skin";
        case SlotType::Blood:
            return "Blood";
    }

    return "Unknown";
}

constexpr const char* SlotShortLabel(const SlotType slot) {
    switch (slot) {
        case SlotType::Head:
            return "HD";
        case SlotType::Torso:
            return "TR";
        case SlotType::LeftArm:
            return "LA";
        case SlotType::RightArm:
            return "RA";
        case SlotType::LeftLeg:
            return "LL";
        case SlotType::RightLeg:
            return "RL";
        case SlotType::Heart:
            return "HT";
        case SlotType::Lungs:
            return "LG";
        case SlotType::Stomach:
            return "ST";
        case SlotType::Spine:
            return "SP";
        case SlotType::Skin:
            return "SK";
        case SlotType::Blood:
            return "BL";
    }

    return "--";
}

constexpr const char* RarityName(const int rarity) {
    switch (rarity) {
        case 0:
            return "Common";
        case 1:
            return "Uncommon";
        case 2:
            return "Rare";
        case 3:
            return "Elite";
        case 4:
            return "Monstrous";
        case 5:
            return "Mythic";
        default:
            return "Unknown";
    }
}

constexpr SDL_Color RarityColor(const int rarity) {
    switch (rarity) {
        case 0:
            return SDL_Color{118, 118, 118, 255};
        case 1:
            return SDL_Color{94, 162, 110, 255};
        case 2:
            return SDL_Color{82, 138, 196, 255};
        case 3:
            return SDL_Color{184, 122, 65, 255};
        case 4:
            return SDL_Color{166, 64, 152, 255};
        case 5:
            return SDL_Color{214, 195, 90, 255};
        default:
            return SDL_Color{160, 160, 160, 255};
    }
}

inline float Clamp(const float value, const float minValue, const float maxValue) {
    return std::clamp(value, minValue, maxValue);
}

inline float RandomFloat(std::mt19937& rng, const float minValue, const float maxValue) {
    std::uniform_real_distribution<float> distribution(minValue, maxValue);
    return distribution(rng);
}

inline int RandomInt(std::mt19937& rng, const int minValue, const int maxValue) {
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(rng);
}

inline void SetDrawColor(SDL_Renderer* renderer, const SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

inline void FillRect(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color color) {
    SetDrawColor(renderer, color);
    SDL_RenderFillRect(renderer, &rect);
}

inline void DrawRect(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color color) {
    SetDrawColor(renderer, color);
    SDL_RenderRect(renderer, &rect);
}

inline void DrawText(SDL_Renderer* renderer, const float x, const float y, const SDL_Color color,
                     const std::string_view text) {
    SetDrawColor(renderer, color);
    SDL_RenderDebugText(renderer, x, y, std::string(text).c_str());
}

inline void DrawBar(SDL_Renderer* renderer, const SDL_FRect& rect, const float ratio,
                    const SDL_Color fillColor, const SDL_Color backgroundColor) {
    FillRect(renderer, rect, backgroundColor);

    SDL_FRect fillRect = rect;
    fillRect.w *= Clamp(ratio, 0.0F, 1.0F);
    FillRect(renderer, fillRect, fillColor);
    DrawRect(renderer, rect, SDL_Color{220, 220, 220, 255});
}

inline std::string FormatNumber(const float value, const int precision = 1) {
    std::ostringstream stream;
    stream.setf(std::ios::fixed);
    stream.precision(precision);
    stream << value;
    return stream.str();
}

inline std::string FitTextToWidth(const std::string_view text, const float maxWidth) {
    if (maxWidth <= 0.0F) {
        return "";
    }

    const int maxChars = std::max(1, static_cast<int>(std::floor(maxWidth / 8.0F)));
    if (static_cast<int>(text.size()) <= maxChars) {
        return std::string(text);
    }

    if (maxChars <= 3) {
        return std::string(static_cast<std::size_t>(maxChars), '.');
    }

    return std::string(text.substr(0, static_cast<std::size_t>(maxChars - 3))) + "...";
}

inline std::vector<std::string> WrapTextToWidth(const std::string_view text, const float maxWidth,
                                                const int maxLines = -1) {
    std::vector<std::string> lines;
    if (text.empty() || maxWidth <= 0.0F) {
        return lines;
    }

    const int maxChars = std::max(1, static_cast<int>(std::floor(maxWidth / 8.0F)));
    std::string remaining(text);

    while (!remaining.empty() && (maxLines < 0 || static_cast<int>(lines.size()) < maxLines)) {
        if (static_cast<int>(remaining.size()) <= maxChars) {
            lines.push_back(remaining);
            break;
        }

        std::size_t breakPos = remaining.rfind(' ', static_cast<std::size_t>(maxChars));
        if (breakPos == std::string::npos || breakPos == 0) {
            breakPos = static_cast<std::size_t>(maxChars);
        }

        std::string line = remaining.substr(0, breakPos);
        while (!line.empty() && line.back() == ' ') {
            line.pop_back();
        }
        lines.push_back(line);

        remaining.erase(0, breakPos);
        while (!remaining.empty() && remaining.front() == ' ') {
            remaining.erase(remaining.begin());
        }
    }

    if (!remaining.empty() && maxLines > 0 && static_cast<int>(lines.size()) == maxLines) {
        lines.back() = FitTextToWidth(lines.back() + " " + remaining, maxWidth);
    }

    return lines;
}

inline void DrawTextFit(SDL_Renderer* renderer, const float x, const float y, const float maxWidth,
                        const SDL_Color color, const std::string_view text) {
    DrawText(renderer, x, y, color, FitTextToWidth(text, maxWidth));
}

inline void DrawWrappedText(SDL_Renderer* renderer, const float x, const float y,
                            const float maxWidth, const int maxLines, const float lineHeight,
                            const SDL_Color color, const std::string_view text) {
    const auto lines = WrapTextToWidth(text, maxWidth, maxLines);
    float lineY = y;
    for (const std::string& line : lines) {
        DrawText(renderer, x, lineY, color, line);
        lineY += lineHeight;
    }
}

inline std::string BuildPartName(const EnemyKind source, const SlotType slot) {
    std::ostringstream stream;
    switch (source) {
        case EnemyKind::Scavenger:
            stream << "Scavenged ";
            break;
        case EnemyKind::Wolf:
            stream << "Wolf ";
            break;
        case EnemyKind::Cultist:
            stream << "Ashen ";
            break;
        case EnemyKind::FailedHomunculus:
            stream << "Twisted ";
            break;
    }

    stream << SlotLabel(slot);
    return stream.str();
}

inline std::array<SDL_FRect, kSlotCount> BuildSlotRects(const SDL_FRect& panel) {
    const float left = panel.x + 26.0F;
    const float top = panel.y + 156.0F;
    const float unit = 54.0F;

    return {
        SDL_FRect{left + unit * 2.0F, top, unit, unit},
        SDL_FRect{left + unit * 2.0F, top + unit * 1.15F, unit, unit * 1.2F},
        SDL_FRect{left + unit * 0.7F, top + unit * 1.2F, unit, unit},
        SDL_FRect{left + unit * 3.3F, top + unit * 1.2F, unit, unit},
        SDL_FRect{left + unit * 1.5F, top + unit * 2.55F, unit, unit * 1.2F},
        SDL_FRect{left + unit * 2.5F, top + unit * 2.55F, unit, unit * 1.2F},
        SDL_FRect{left + unit * 1.1F, top + unit * 1.7F, unit * 0.75F, unit * 0.75F},
        SDL_FRect{left + unit * 2.9F, top + unit * 1.7F, unit * 0.75F, unit * 0.75F},
        SDL_FRect{left + unit * 2.0F, top + unit * 2.15F, unit, unit * 0.8F},
        SDL_FRect{left + unit * 2.0F, top + unit * 1.2F, unit * 0.3F, unit * 2.35F},
        SDL_FRect{left + unit * 0.9F, top + unit * 1.05F, unit * 2.8F, unit * 1.55F},
        SDL_FRect{left + unit * 1.0F, top + unit * 3.9F, unit * 3.0F, unit * 0.45F},
    };
}

inline SDL_FRect PauseModalRect() { return SDL_FRect{440.0F, 210.0F, 400.0F, 250.0F}; }

inline SDL_FRect ResumeButtonRect() {
    const SDL_FRect modal = PauseModalRect();
    return SDL_FRect{modal.x + 70.0F, modal.y + 138.0F, 260.0F, 40.0F};
}

inline SDL_FRect RestartButtonRect() {
    const SDL_FRect modal = PauseModalRect();
    return SDL_FRect{modal.x + 70.0F, modal.y + 190.0F, 260.0F, 40.0F};
}

inline SDL_FRect TitleStartButtonRect() { return SDL_FRect{470.0F, 526.0F, 340.0F, 48.0F}; }

inline SDL_FRect SkillTreeModalRect() { return SDL_FRect{110.0F, 74.0F, 1060.0F, 572.0F}; }

inline SDL_FRect SkillTreeGraphRect() {
    const SDL_FRect modal = SkillTreeModalRect();
    return SDL_FRect{modal.x + 24.0F, modal.y + 70.0F, 728.0F, 430.0F};
}

inline SDL_FRect SkillTreeDetailsRect() {
    const SDL_FRect graph = SkillTreeGraphRect();
    return SDL_FRect{graph.x + graph.w + 24.0F, graph.y, 260.0F, graph.h};
}

inline SDL_FRect SkillNodeRect(const int index) {
    const SDL_FRect graph = SkillTreeGraphRect();
    const SkillNodeDefinition& definition = SkillDefinition(index);
    return SDL_FRect{graph.x + definition.position.x, graph.y + definition.position.y, 186.0F,
                     84.0F};
}

inline SDL_FPoint SkillNodeCenter(const int index) {
    const SDL_FRect rect = SkillNodeRect(index);
    return SDL_FPoint{rect.x + rect.w * 0.5F, rect.y + rect.h * 0.5F};
}

inline SDL_FRect SkillSpendButtonRect() {
    const SDL_FRect details = SkillTreeDetailsRect();
    return SDL_FRect{details.x + 18.0F, details.y + details.h - 102.0F, details.w - 36.0F, 40.0F};
}

inline SDL_FRect SkillCloseButtonRect() {
    const SDL_FRect details = SkillTreeDetailsRect();
    return SDL_FRect{details.x + 18.0F, details.y + details.h - 52.0F, details.w - 36.0F, 36.0F};
}

inline SDL_FRect RunEndPrimaryButtonRect() { return SDL_FRect{438.0F, 426.0F, 404.0F, 44.0F}; }

inline SDL_FRect RunEndSecondaryButtonRect() { return SDL_FRect{438.0F, 482.0F, 404.0F, 44.0F}; }

inline bool PointInRect(const SDL_FRect& rect, const float x, const float y) {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

inline SDL_FRect InsetRect(const SDL_FRect& rect, const float inset) {
    return SDL_FRect{rect.x + inset, rect.y + inset, rect.w - inset * 2.0F, rect.h - inset * 2.0F};
}

inline void DrawPanelChrome(SDL_Renderer* renderer, const SDL_FRect& rect,
                            const SDL_Color fillColor, const SDL_Color borderColor,
                            const SDL_Color accentColor) {
    FillRect(renderer, SDL_FRect{rect.x + 4.0F, rect.y + 6.0F, rect.w, rect.h},
             SDL_Color{0, 0, 0, 74});
    FillRect(renderer, rect, fillColor);
    FillRect(renderer, SDL_FRect{rect.x, rect.y, rect.w, 5.0F}, accentColor);
    DrawRect(renderer, rect, borderColor);
    DrawRect(renderer, InsetRect(rect, 6.0F), SDL_Color{255, 255, 255, 18});
}

inline void DrawButtonChrome(SDL_Renderer* renderer, const SDL_FRect& rect,
                             const SDL_Color fillColor, const SDL_Color borderColor,
                             const SDL_Color accentColor, const std::string_view label,
                             const bool selected = false) {
    FillRect(renderer, SDL_FRect{rect.x + 4.0F, rect.y + 5.0F, rect.w, rect.h},
             SDL_Color{0, 0, 0, 86});
    FillRect(renderer, rect, fillColor);
    FillRect(renderer, SDL_FRect{rect.x, rect.y, rect.w, 5.0F}, accentColor);
    DrawRect(renderer, rect, selected ? SDL_Color{248, 244, 232, 255} : borderColor);
    DrawTextFit(renderer, rect.x + 16.0F, rect.y + rect.h * 0.5F - 5.0F, rect.w - 32.0F,
                SDL_Color{18, 18, 18, 255}, label);
}

inline void DrawTag(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color fillColor,
                    const SDL_Color borderColor, const std::string_view label,
                    const SDL_Color textColor = SDL_Color{18, 18, 18, 255}) {
    FillRect(renderer, rect, fillColor);
    DrawRect(renderer, rect, borderColor);
    DrawTextFit(renderer, rect.x + 8.0F, rect.y + rect.h * 0.5F - 5.0F, rect.w - 16.0F, textColor,
                label);
}

inline void RenderEnemyFigure(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color color,
                              const bool elite) {
    const SDL_Color dark{static_cast<Uint8>(std::max(0, color.r - 50)),
                         static_cast<Uint8>(std::max(0, color.g - 42)),
                         static_cast<Uint8>(std::max(0, color.b - 42)), 255};

    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.34F, rect.y + rect.h * 0.12F, rect.w * 0.32F, rect.h * 0.22F},
        color);
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.28F, rect.y + rect.h * 0.34F, rect.w * 0.44F, rect.h * 0.34F},
        dark);
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.2F, rect.y + rect.h * 0.42F, rect.w * 0.14F, rect.h * 0.24F},
        color);
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.66F, rect.y + rect.h * 0.42F, rect.w * 0.14F, rect.h * 0.24F},
        color);
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.34F, rect.y + rect.h * 0.68F, rect.w * 0.12F, rect.h * 0.22F},
        color);
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.54F, rect.y + rect.h * 0.68F, rect.w * 0.12F, rect.h * 0.22F},
        color);
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.43F, rect.y + rect.h * 0.2F, rect.w * 0.05F, rect.h * 0.04F},
        SDL_Color{92, 212, 114, 255});
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.53F, rect.y + rect.h * 0.2F, rect.w * 0.05F, rect.h * 0.04F},
        SDL_Color{92, 212, 114, 255});

    if (elite) {
        DrawRect(renderer, SDL_FRect{rect.x + 8.0F, rect.y + 8.0F, rect.w - 16.0F, rect.h - 16.0F},
                 SDL_Color{226, 194, 88, 255});
    }
}

inline void RenderCorpsePile(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color color) {
    FillRect(renderer, SDL_FRect{rect.x + 10.0F, rect.y + 18.0F, rect.w - 20.0F, rect.h - 18.0F},
             SDL_Color{static_cast<Uint8>(std::max(0, color.r - 35)),
                       static_cast<Uint8>(std::max(0, color.g - 28)),
                       static_cast<Uint8>(std::max(0, color.b - 30)), 255});
    FillRect(renderer, SDL_FRect{rect.x + 18.0F, rect.y + 10.0F, rect.w * 0.36F, rect.h * 0.28F},
             color);
    FillRect(renderer,
             SDL_FRect{rect.x + rect.w * 0.5F, rect.y + 6.0F, rect.w * 0.24F, rect.h * 0.25F},
             SDL_Color{color.r, static_cast<Uint8>(std::max(0, color.g - 10)), color.b, 255});
    DrawRect(renderer, rect, SDL_Color{210, 208, 204, 255});
}

}  // namespace fleshgolem_internal
