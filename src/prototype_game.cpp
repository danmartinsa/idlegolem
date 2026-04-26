#include "prototype_game.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string_view>

namespace {
constexpr std::size_t kSlotCount = 12;
constexpr std::size_t kEnemyTypeCount = 4;
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
    int kills = 0;
    int grafts = 0;
    int salvaged = 0;
};

struct RunState {
    int depth = 1;
    int encounters = 0;
    int eliteCountdown = 4;
    int highestTier = 1;
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
};

struct Corpse {
    EnemyKind source = EnemyKind::Scavenger;
    int tier = 1;
    bool elite = false;
    float harvestTimer = 1.0F;
    float maxHarvestTimer = 1.0F;
    int biomassReward = 0;
    int boneMealReward = 0;
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

constexpr std::array<SlotType, kSlotCount> kAllSlots{
    SlotType::Head,    SlotType::Torso,    SlotType::LeftArm, SlotType::RightArm,
    SlotType::LeftLeg, SlotType::RightLeg, SlotType::Heart,   SlotType::Lungs,
    SlotType::Stomach, SlotType::Spine,    SlotType::Skin,    SlotType::Blood,
};

constexpr EnemyTemplate EnemyTemplateFor(const EnemyKind kind) {
    switch (kind) {
        case EnemyKind::Scavenger:
            return {
                .kind = kind,
                .name = "Grave Scavenger",
                .color = SDL_Color{164, 108, 96, 255},
                .health = 42.0F,
                .attack = 8.0F,
                .attackInterval = 1.45F,
                .armor = 1.0F,
                .evasion = 0.05F,
                .lootSlots = {SlotType::LeftArm, SlotType::RightArm, SlotType::Head,
                              SlotType::Torso},
                .lootSlotCount = 4,
            };
        case EnemyKind::Wolf:
            return {
                .kind = kind,
                .name = "Carrion Wolf",
                .color = SDL_Color{105, 138, 112, 255},
                .health = 36.0F,
                .attack = 9.0F,
                .attackInterval = 1.18F,
                .armor = 0.5F,
                .evasion = 0.12F,
                .lootSlots = {SlotType::LeftLeg, SlotType::RightLeg, SlotType::Skin,
                              SlotType::Head},
                .lootSlotCount = 4,
            };
        case EnemyKind::Cultist:
            return {
                .kind = kind,
                .name = "Ash Chapel Cultist",
                .color = SDL_Color{122, 92, 154, 255},
                .health = 48.0F,
                .attack = 10.0F,
                .attackInterval = 1.32F,
                .armor = 1.8F,
                .evasion = 0.08F,
                .lootSlots = {SlotType::Head, SlotType::Lungs, SlotType::Blood, SlotType::Heart},
                .lootSlotCount = 4,
            };
        case EnemyKind::FailedHomunculus:
            return {
                .kind = kind,
                .name = "Failed Homunculus",
                .color = SDL_Color{180, 72, 88, 255},
                .health = 62.0F,
                .attack = 11.0F,
                .attackInterval = 1.36F,
                .armor = 2.8F,
                .evasion = 0.04F,
                .lootSlots = {SlotType::Heart, SlotType::Spine, SlotType::Torso, SlotType::LeftArm},
                .lootSlotCount = 4,
            };
    }

    return EnemyTemplateFor(EnemyKind::Scavenger);
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

float Clamp(const float value, const float minValue, const float maxValue) {
    return std::clamp(value, minValue, maxValue);
}

float RandomFloat(std::mt19937& rng, const float minValue, const float maxValue) {
    std::uniform_real_distribution<float> distribution(minValue, maxValue);
    return distribution(rng);
}

int RandomInt(std::mt19937& rng, const int minValue, const int maxValue) {
    std::uniform_int_distribution<int> distribution(minValue, maxValue);
    return distribution(rng);
}

void SetDrawColor(SDL_Renderer* renderer, const SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void FillRect(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color color) {
    SetDrawColor(renderer, color);
    SDL_RenderFillRect(renderer, &rect);
}

void DrawRect(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color color) {
    SetDrawColor(renderer, color);
    SDL_RenderRect(renderer, &rect);
}

void DrawText(SDL_Renderer* renderer, const float x, const float y, const SDL_Color color,
              const std::string_view text) {
    SetDrawColor(renderer, color);
    SDL_RenderDebugText(renderer, x, y, std::string(text).c_str());
}

void DrawBar(SDL_Renderer* renderer, const SDL_FRect& rect, const float ratio,
             const SDL_Color fillColor, const SDL_Color backgroundColor) {
    FillRect(renderer, rect, backgroundColor);

    SDL_FRect fillRect = rect;
    fillRect.w *= Clamp(ratio, 0.0F, 1.0F);
    FillRect(renderer, fillRect, fillColor);
    DrawRect(renderer, rect, SDL_Color{220, 220, 220, 255});
}

std::string FormatNumber(const float value, const int precision = 1) {
    std::ostringstream stream;
    stream.setf(std::ios::fixed);
    stream.precision(precision);
    stream << value;
    return stream.str();
}

std::string BuildPartName(const EnemyKind source, const SlotType slot) {
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

std::array<SDL_FRect, kSlotCount> BuildSlotRects(const SDL_FRect& panel) {
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

void RenderEnemyFigure(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color color,
                       const bool elite) {
    const SDL_Color outline{230, 224, 216, 255};
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
        FillRect(renderer,
                 SDL_FRect{rect.x + rect.w * 0.36F, rect.y + rect.h * 0.04F, rect.w * 0.1F,
                           rect.h * 0.1F},
                 SDL_Color{214, 176, 76, 255});
        FillRect(renderer,
                 SDL_FRect{rect.x + rect.w * 0.54F, rect.y + rect.h * 0.04F, rect.w * 0.1F,
                           rect.h * 0.1F},
                 SDL_Color{214, 176, 76, 255});
    }

    DrawRect(renderer, rect, outline);
}

void RenderCorpsePile(SDL_Renderer* renderer, const SDL_FRect& rect, const SDL_Color color) {
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.14F, rect.y + rect.h * 0.52F, rect.w * 0.72F, rect.h * 0.22F},
        color);
    FillRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.26F, rect.y + rect.h * 0.34F, rect.w * 0.4F, rect.h * 0.2F},
        SDL_Color{static_cast<Uint8>(std::max(0, color.r - 28)),
                  static_cast<Uint8>(std::max(0, color.g - 22)),
                  static_cast<Uint8>(std::max(0, color.b - 22)), 255});
    DrawRect(
        renderer,
        SDL_FRect{rect.x + rect.w * 0.12F, rect.y + rect.h * 0.32F, rect.w * 0.76F, rect.h * 0.44F},
        SDL_Color{218, 216, 210, 255});
}
}  // namespace

PrototypeGame::PrototypeGame(SDL_Renderer* renderer)
    : renderer_(renderer), rng_(std::random_device{}()) {
    LoadAssets();
    ResetRun();
}

PrototypeGame::~PrototypeGame() { DestroyAssets(); }

bool PrototypeGame::LoadAssets() {
    DestroyAssets();

    auto loadSpriteSheet = [this](const std::string& fileName,
                                  SDL_Texture** textureTarget,
                                  std::string* pathTarget) {
        constexpr std::array<const char*, 2> kAssetRoots{"assets/", "../assets/"};

        for (const char* assetRoot : kAssetRoots) {
            const std::string candidatePath = std::string(assetRoot) + fileName;
            SDL_Surface* surface = SDL_LoadBMP(candidatePath.c_str());
            if (surface == nullptr) {
                continue;
            }

            const Uint32 colorKey = SDL_MapSurfaceRGB(surface, 255, 0, 255);
            SDL_SetSurfaceColorKey(surface, true, colorKey);

            *textureTarget = SDL_CreateTextureFromSurface(renderer_, surface);
            SDL_DestroySurface(surface);

            if (*textureTarget != nullptr) {
                SDL_SetTextureScaleMode(*textureTarget, SDL_SCALEMODE_NEAREST);
                *pathTarget = candidatePath;
                return true;
            }
        }

        pathTarget->clear();
        return false;
    };

    const bool playerLoaded =
        loadSpriteSheet("player_walk.bmp", &playerSpriteSheet_, &spriteSheetPath_);
    if (!playerLoaded) {
        SDL_Log("Failed to load player sprite sheet: %s", SDL_GetError());
    }

    bool allEnemySpritesLoaded = true;
    for (std::size_t index = 0; index < kEnemyTypeCount; ++index) {
        const EnemyKind kind = static_cast<EnemyKind>(index);
        if (!loadSpriteSheet(
                EnemySpriteFileName(kind), &enemySpriteSheets_[index], &enemySpriteSheetPaths_[index])) {
            SDL_Log("Failed to load enemy sprite sheet for %s: %s",
                    EnemyTemplateFor(kind).name,
                    SDL_GetError());
            allEnemySpritesLoaded = false;
        }
    }

    return playerLoaded && allEnemySpritesLoaded;
}

void PrototypeGame::DestroyAssets() {
    if (playerSpriteSheet_ != nullptr) {
        SDL_DestroyTexture(playerSpriteSheet_);
        playerSpriteSheet_ = nullptr;
    }

    for (SDL_Texture*& texture : enemySpriteSheets_) {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
}

void PrototypeGame::HandleEvent(const SDL_Event& event) {
    if (event.type != SDL_EVENT_KEY_DOWN || event.key.repeat) {
        return;
    }

    switch (event.key.key) {
        case SDLK_SPACE:
            paused_ = !paused_;
            AddLog(paused_ ? "Systems paused." : "Systems resumed.");
            break;
        case SDLK_R:
            AddLog("The homunculus is restitched from scraps.");
            ResetRun();
            break;
        case SDLK_E:
            DissolveRun();
            break;
        case SDLK_N:
            if (currentEnemy_ == entt::null && currentCorpse_ == entt::null && !runFailed_) {
                enemySpawnDelay_ = 0.0F;
                AddLog("Scouting accelerates the next encounter.");
            }
            break;
        default:
            break;
    }
}

void PrototypeGame::Update(float deltaSeconds) {
    const float clampedDelta = Clamp(deltaSeconds, 0.0F, kMaxDeltaSeconds);
    RecalculatePlayerStats();

    if (!paused_) {
        UpdateAnimation(clampedDelta);
    }

    if (!paused_ && !runFailed_) {
        UpdateDecay(clampedDelta);
        UpdateHarvesting(clampedDelta);
        SpawnEnemyIfNeeded(clampedDelta);
        UpdateLane(clampedDelta);
        UpdateCombat(clampedDelta);
        RecalculatePlayerStats();
    }

    UpdateWindowTitle();
}

void PrototypeGame::Render() const {
    RenderBackdrop();

    RenderResourcePanel(SDL_FRect{24.0F, 20.0F, 1232.0F, 78.0F});
    RenderPlayerPanel(SDL_FRect{24.0F, 118.0F, 470.0F, 578.0F});
    RenderEncounterPanel(SDL_FRect{520.0F, 118.0F, 736.0F, 340.0F});
    RenderEventLog(SDL_FRect{520.0F, 480.0F, 736.0F, 216.0F});
}

const std::string& PrototypeGame::WindowTitle() const { return windowTitle_; }

void PrototypeGame::ResetRun() {
    registry_.clear();
    player_ = entt::null;
    currentEnemy_ = entt::null;
    currentCorpse_ = entt::null;
    enemySpawnDelay_ = 0.85F;
    enemyLaneX_ = kLaneEnemySpawnX;
    enemyApproachSpeed_ = 0.0F;
    corpseLaneX_ = kLaneEnemyContactX;
    playerAnimationTime_ = 0.0F;
    playerAnimationFrame_ = 0;
    combatJoined_ = false;
    paused_ = false;
    runFailed_ = false;
    eventLog_.clear();

    SpawnPlayer();
    BuildStarterBody();
    RecalculatePlayerStats();
    AddLog("Fresh homunculus awakened in the Charnel Pits.");
    if (!spriteSheetPath_.empty()) {
        AddLog("Loaded walk sprite sheet: " + spriteSheetPath_);
    } else {
        AddLog("Walk sprite sheet missing; using fallback render.");
    }
}

void PrototypeGame::SpawnPlayer() {
    player_ = registry_.create();

    const int marrowRanks = bankedEssence_ / 6;
    registry_.emplace<Name>(player_, "Prototype Homunculus");
    registry_.emplace<PlayerTag>(player_);
    registry_.emplace<BaseAttributes>(player_,
                                      BaseAttributes{
                                          .might = 4.0F + static_cast<float>(marrowRanks / 2),
                                          .vitality = 5.0F + static_cast<float>(marrowRanks / 2),
                                          .agility = 3.0F + static_cast<float>(marrowRanks / 3),
                                          .reason = 2.0F + static_cast<float>(marrowRanks / 4),
                                          .instinct = 3.0F + static_cast<float>(marrowRanks / 3),
                                          .corruption = 0.0F,
                                      });
    registry_.emplace<Attributes>(player_);
    registry_.emplace<CombatStats>(player_);
    registry_.emplace<Health>(player_);
    registry_.emplace<CombatTimer>(player_, CombatTimer{0.4F});
    registry_.emplace<Body>(player_);
    registry_.emplace<Resources>(player_);
    registry_.emplace<RunState>(player_);
    registry_.emplace<StabilityState>(player_);
}

void PrototypeGame::BuildStarterBody() {
    auto createStarterPart =
        [this](const std::string& name, const SlotType slot, const Attributes bonus,
               const float healthBonus, const float attackBonus, const float attackIntervalBonus,
               const float critChanceBonus, const float armorBonus, const float evasionBonus,
               const float decayRate, const float stabilityDemand, const SDL_Color color) {
            const entt::entity part = registry_.create();
            registry_.emplace<BodyPart>(part, BodyPart{
                                                  .name = name,
                                                  .slot = slot,
                                                  .rarity = 0,
                                                  .attributeBonus = bonus,
                                                  .healthBonus = healthBonus,
                                                  .attackBonus = attackBonus,
                                                  .attackIntervalBonus = attackIntervalBonus,
                                                  .critChanceBonus = critChanceBonus,
                                                  .armorBonus = armorBonus,
                                                  .evasionBonus = evasionBonus,
                                                  .freshness = 100.0F,
                                                  .decayRate = decayRate,
                                                  .stabilityDemand = stabilityDemand,
                                                  .color = color,
                                              });
            registry_.emplace<Name>(part, name);
            registry_.emplace<EquippedTo>(part, player_, slot);
            registry_.get<Body>(player_).slots[SlotIndex(slot)] = part;
        };

    createStarterPart("Stitched Skull", SlotType::Head,
                      Attributes{.reason = 0.4F, .instinct = 1.0F}, 0.0F, 0.0F, 0.0F, 0.015F, 0.0F,
                      0.01F, 0.08F, 0.9F, SDL_Color{105, 105, 124, 255});
    createStarterPart("Patchwork Torso", SlotType::Torso, Attributes{.vitality = 2.0F}, 24.0F, 0.0F,
                      0.0F, 0.0F, 1.4F, 0.0F, 0.07F, 1.2F, SDL_Color{126, 88, 82, 255});
    createStarterPart("Scrap Left Arm", SlotType::LeftArm, Attributes{.might = 1.0F}, 0.0F, 2.0F,
                      -0.03F, 0.0F, 0.0F, 0.0F, 0.09F, 0.8F, SDL_Color{142, 112, 98, 255});
    createStarterPart("Scrap Right Arm", SlotType::RightArm, Attributes{.might = 1.0F}, 0.0F, 2.0F,
                      -0.03F, 0.0F, 0.0F, 0.0F, 0.09F, 0.8F, SDL_Color{142, 112, 98, 255});
    createStarterPart("Bent Left Leg", SlotType::LeftLeg, Attributes{.agility = 1.0F}, 0.0F, 0.0F,
                      -0.02F, 0.0F, 0.0F, 0.015F, 0.09F, 0.8F, SDL_Color{108, 126, 100, 255});
    createStarterPart("Bent Right Leg", SlotType::RightLeg, Attributes{.agility = 1.0F}, 0.0F, 0.0F,
                      -0.02F, 0.0F, 0.0F, 0.015F, 0.09F, 0.8F, SDL_Color{108, 126, 100, 255});
    createStarterPart("Donor Heart", SlotType::Heart, Attributes{.vitality = 1.0F}, 18.0F, 0.0F,
                      0.0F, 0.0F, 0.0F, 0.0F, 0.08F, 1.0F, SDL_Color{172, 72, 84, 255});
    createStarterPart("Threadbare Lungs", SlotType::Lungs,
                      Attributes{.vitality = 0.5F, .agility = 0.7F}, 0.0F, 0.0F, -0.01F, 0.0F, 0.0F,
                      0.01F, 0.08F, 0.8F, SDL_Color{132, 144, 160, 255});
}

void PrototypeGame::SpawnEnemyIfNeeded(const float deltaSeconds) {
    if (currentEnemy_ != entt::null || currentCorpse_ != entt::null || runFailed_) {
        return;
    }

    enemySpawnDelay_ -= deltaSeconds;
    if (enemySpawnDelay_ > 0.0F) {
        return;
    }

    const RunState& runState = registry_.get<RunState>(player_);
    const int tier = 1 + (runState.depth - 1) / 4;
    const bool elite = runState.eliteCountdown <= 0;
    SpawnEnemy(tier, elite);
}

void PrototypeGame::SpawnEnemy(const int tier, const bool elite) {
    EnemyKind kind = EnemyKind::Scavenger;
    const int roll = RandomInt(rng_, 0, 99);

    if (tier <= 1) {
        kind = (roll < 55) ? EnemyKind::Scavenger : EnemyKind::Wolf;
    } else if (tier == 2) {
        if (roll < 40) {
            kind = EnemyKind::Scavenger;
        } else if (roll < 75) {
            kind = EnemyKind::Wolf;
        } else {
            kind = EnemyKind::Cultist;
        }
    } else {
        if (roll < 25) {
            kind = EnemyKind::Wolf;
        } else if (roll < 55) {
            kind = EnemyKind::Cultist;
        } else {
            kind = EnemyKind::FailedHomunculus;
        }
    }

    const EnemyTemplate enemyTemplate = EnemyTemplateFor(kind);
    const float scale = 1.0F + static_cast<float>(tier - 1) * 0.35F +
                        static_cast<float>(registry_.get<RunState>(player_).depth - 1) * 0.06F +
                        (elite ? 0.75F : 0.0F);

    currentEnemy_ = registry_.create();
    registry_.emplace<Name>(currentEnemy_, elite ? std::string("Elite ") + enemyTemplate.name
                                                 : std::string(enemyTemplate.name));
    registry_.emplace<EnemyTag>(currentEnemy_);
    registry_.emplace<Enemy>(currentEnemy_, Enemy{.kind = kind, .tier = tier, .elite = elite});
    registry_.emplace<CombatStats>(
        currentEnemy_,
        CombatStats{
            .attackPower = enemyTemplate.attack * scale,
            .attackInterval = Clamp(
                enemyTemplate.attackInterval - static_cast<float>(tier - 1) * 0.04F, 0.7F, 1.8F),
            .critChance = elite ? 0.12F : 0.06F,
            .critMultiplier = elite ? 1.75F : 1.5F,
            .armor =
                enemyTemplate.armor + static_cast<float>(tier - 1) * 0.7F + (elite ? 1.5F : 0.0F),
            .evasion = Clamp(enemyTemplate.evasion + (elite ? 0.04F : 0.0F), 0.0F, 0.28F),
        });
    registry_.emplace<Health>(currentEnemy_, Health{.current = enemyTemplate.health * scale,
                                                    .maximum = enemyTemplate.health * scale});
    registry_.emplace<CombatTimer>(currentEnemy_, CombatTimer{0.4F});

    auto& runState = registry_.get<RunState>(player_);
    runState.encounters++;
    runState.highestTier = std::max(runState.highestTier, tier);

    enemyLaneX_ = kLaneEnemySpawnX;
    enemyApproachSpeed_ = 118.0F + static_cast<float>(tier) * 14.0F + (elite ? 22.0F : 0.0F);
    combatJoined_ = false;

    std::ostringstream logLine;
    logLine << "Encounter " << runState.encounters << ": "
            << registry_.get<Name>(currentEnemy_).value << " emerges from the right flank.";
    AddLog(logLine.str());
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
    registry_.get<CombatTimer>(player_).attackCooldown = 0.15F;
    registry_.get<CombatTimer>(currentEnemy_).attackCooldown = 0.55F;
    AddLog(registry_.get<Name>(currentEnemy_).value + " reaches the graft line. Combat starts.");
}

void PrototypeGame::RecalculatePlayerStats() {
    if (player_ == entt::null || !registry_.valid(player_)) {
        return;
    }

    const BaseAttributes& base = registry_.get<BaseAttributes>(player_);
    Attributes current{
        .might = base.might,
        .vitality = base.vitality,
        .agility = base.agility,
        .reason = base.reason,
        .instinct = base.instinct,
        .corruption = base.corruption,
    };

    float bonusHealth = 0.0F;
    float bonusAttack = 0.0F;
    float intervalBonus = 0.0F;
    float critBonus = 0.0F;
    float armorBonus = 0.0F;
    float evasionBonus = 0.0F;
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

    StabilityState stability{};
    stability.capacity = 6.0F + current.reason * 0.85F + current.vitality * 0.45F;
    stability.demand = stabilityDemand;
    stability.overload = std::max(0.0F, stability.demand - stability.capacity);
    registry_.replace<StabilityState>(player_, stability);

    CombatStats stats{};
    stats.attackPower =
        std::max(3.5F, 6.0F + current.might * 2.45F + bonusAttack - stability.overload * 1.15F);
    stats.attackInterval = Clamp(
        1.45F - current.agility * 0.05F + intervalBonus + stability.overload * 0.05F, 0.45F, 1.8F);
    stats.critChance = Clamp(
        0.03F + current.instinct * 0.01F + critBonus - stability.overload * 0.01F, 0.01F, 0.45F);
    stats.critMultiplier = 1.45F + current.reason * 0.035F;
    stats.armor = std::max(0.0F, current.vitality * 0.52F + armorBonus - stability.overload * 0.4F);
    stats.evasion =
        Clamp(current.agility * 0.012F + evasionBonus - stability.overload * 0.01F, 0.0F, 0.35F);

    Health& health = registry_.get<Health>(player_);
    const float previousMaximum = std::max(1.0F, health.maximum);
    const float missingHealth = std::max(0.0F, previousMaximum - health.current);
    health.maximum =
        std::max(30.0F, 72.0F + current.vitality * 16.0F + bonusHealth - stability.overload * 8.0F);
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

    while (!runFailed_ && currentEnemy_ != entt::null && registry_.valid(currentEnemy_) &&
           playerTimer.attackCooldown <= 0.0F) {
        ResolveAttack(player_, currentEnemy_);
        playerTimer.attackCooldown += playerStats.attackInterval;
    }

    while (!runFailed_ && currentEnemy_ != entt::null && registry_.valid(currentEnemy_) &&
           enemyTimer.attackCooldown <= 0.0F) {
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
        runFailed_ = true;
        combatJoined_ = false;
        AddLog("The homunculus collapses into useless grafts.");
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
    resources.essence += 1 + enemyInfo.tier + (enemyInfo.elite ? 2 : 0);
    runState.depth++;

    if (enemyInfo.elite) {
        runState.eliteCountdown = 4 + std::min(runState.depth / 3, 4);
    } else {
        runState.eliteCountdown--;
    }

    corpseLaneX_ = enemyLaneX_;
    currentCorpse_ = registry_.create();
    registry_.emplace<Name>(currentCorpse_, std::string("Remains of ") + enemyTemplate.name);
    registry_.emplace<Corpse>(
        currentCorpse_,
        Corpse{
            .source = enemyInfo.kind,
            .tier = enemyInfo.tier,
            .elite = enemyInfo.elite,
            .harvestTimer = enemyInfo.elite ? 1.2F : 0.8F,
            .maxHarvestTimer = enemyInfo.elite ? 1.2F : 0.8F,
            .biomassReward = RandomInt(rng_, 5 + enemyInfo.tier * 2,
                                       8 + enemyInfo.tier * 3 + (enemyInfo.elite ? 4 : 0)),
            .boneMealReward = RandomInt(rng_, 2 + enemyInfo.tier,
                                        4 + enemyInfo.tier * 2 + (enemyInfo.elite ? 2 : 0)),
        });

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
    Body& body = registry_.get<Body>(player_);

    resources.biomass += corpse.biomassReward;
    resources.boneMeal += corpse.boneMealReward;

    const EnemyTemplate enemyTemplate = EnemyTemplateFor(corpse.source);
    const bool dropPart = corpse.elite || RandomFloat(rng_, 0.0F, 1.0F) < 0.72F;

    if (dropPart) {
        const SlotType slot =
            enemyTemplate.lootSlots[RandomInt(rng_, 0, enemyTemplate.lootSlotCount - 1)];
        int rarity = std::min(5, corpse.tier - 1 + (corpse.elite ? 2 : 0));
        rarity = std::max(0, rarity + (RandomFloat(rng_, 0.0F, 1.0F) < 0.25F ? 1 : 0));
        rarity = std::min(rarity, 5);
        const float scale =
            1.0F + static_cast<float>(corpse.tier - 1) * 0.35F + static_cast<float>(rarity) * 0.28F;

        Attributes bonus{};
        float healthBonus = 0.0F;
        float attackBonus = 0.0F;
        float intervalBonus = 0.0F;
        float critBonus = 0.0F;
        float armorBonus = 0.0F;
        float evasionBonus = 0.0F;

        switch (slot) {
            case SlotType::Head:
                bonus.reason = 0.5F * scale;
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
        registry_.emplace<Name>(part, partName);
        registry_.emplace<BodyPart>(
            part, BodyPart{
                      .name = partName,
                      .slot = slot,
                      .rarity = rarity,
                      .attributeBonus = bonus,
                      .healthBonus = healthBonus,
                      .attackBonus = attackBonus,
                      .attackIntervalBonus = intervalBonus,
                      .critChanceBonus = critBonus,
                      .armorBonus = armorBonus,
                      .evasionBonus = evasionBonus,
                      .freshness = 100.0F,
                      .decayRate = 0.07F + static_cast<float>(corpse.tier) * 0.015F +
                                   static_cast<float>(rarity) * 0.01F,
                      .stabilityDemand = 0.8F + scale * 0.55F,
                      .color = RarityColor(rarity),
                  });

        const auto evaluatePartScore = [this](const entt::entity entity) {
            const BodyPart& part = registry_.get<BodyPart>(entity);
            const float freshnessFactor = Clamp(part.freshness / 100.0F, 0.15F, 1.0F);
            return freshnessFactor *
                   (part.attackBonus * 4.5F + part.healthBonus * 0.32F +
                    part.attributeBonus.might * 3.0F + part.attributeBonus.vitality * 2.4F +
                    part.attributeBonus.agility * 2.2F + part.attributeBonus.reason * 1.4F +
                    part.attributeBonus.instinct * 1.8F + part.critChanceBonus * 110.0F +
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

            std::ostringstream logLine;
            logLine << "Grafted " << RarityName(rarity) << " " << partName << ".";
            AddLog(logLine.str());
        } else {
            resources.biomass += 2 + rarity * 2;
            resources.salvaged++;
            AddLog("Salvaged spare " + partName + " for Biomass.");
            registry_.destroy(part);
        }
    } else {
        AddLog("The remains yield only slurry and Bone Meal.");
    }

    enemySpawnDelay_ = 1.05F;
    registry_.destroy(corpseEntity);
    currentCorpse_ = entt::null;
}

void PrototypeGame::UpdateDecay(const float deltaSeconds) {
    const float overload = registry_.get<StabilityState>(player_).overload;
    const float multiplier = 1.0F + overload * 0.22F;

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

void PrototypeGame::DissolveRun() {
    if (player_ == entt::null || !registry_.valid(player_)) {
        return;
    }

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);
    const int payout = std::max(1, resources.essence + resources.kills / 2 + resources.grafts / 2 +
                                       resources.boneMeal / 6 + runState.highestTier);

    bankedEssence_ += payout;
    completedRuns_++;
    ResetRun();

    std::ostringstream logLine;
    logLine << "Dissolve complete: +" << payout << " Primordial Essence banked.";
    AddLog(logLine.str());
}

void PrototypeGame::AddLog(const std::string& message) {
    eventLog_.push_front(message);
    while (eventLog_.size() > 9) {
        eventLog_.pop_back();
    }
}

void PrototypeGame::UpdateWindowTitle() {
    if (player_ == entt::null || !registry_.valid(player_)) {
        windowTitle_ = "idlegolem | prototype";
        return;
    }

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);

    std::ostringstream title;
    title << "idlegolem | prototype | depth " << runState.depth << " | kills " << resources.kills
          << " | biomass " << resources.biomass << " | bone meal " << resources.boneMeal
          << " | essence " << resources.essence << " | bank " << bankedEssence_;

    if (paused_) {
        title << " | paused";
    }
    if (runFailed_) {
        title << " | collapsed";
    }

    windowTitle_ = title.str();
}

void PrototypeGame::RenderBackdrop() const {
    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, 1280.0F, 720.0F}, SDL_Color{18, 14, 22, 255});
    FillRect(renderer_, SDL_FRect{0.0F, 0.0F, 1280.0F, 200.0F}, SDL_Color{28, 16, 24, 255});
}

void PrototypeGame::RenderResourcePanel(const SDL_FRect& panel) const {
    FillRect(renderer_, panel, SDL_Color{36, 26, 36, 255});
    DrawRect(renderer_, panel, SDL_Color{110, 92, 110, 255});

    const Resources& resources = registry_.get<Resources>(player_);
    const RunState& runState = registry_.get<RunState>(player_);
    const StabilityState& stability = registry_.get<StabilityState>(player_);

    DrawText(renderer_, panel.x + 16.0F, panel.y + 10.0F, SDL_Color{234, 214, 214, 255},
             "FLESHGOLEM PROTOTYPE  |  Space pause  R restart  E dissolve  N scout");

    std::ostringstream resourceLine;
    resourceLine << "Zone: Charnel Pits   Depth: " << runState.depth
                 << "   Kills: " << resources.kills << "   Biomass: " << resources.biomass
                 << "   Bone Meal: " << resources.boneMeal
                 << "   Run Essence: " << resources.essence
                 << "   Banked Essence: " << bankedEssence_;
    DrawText(renderer_, panel.x + 16.0F, panel.y + 30.0F, SDL_Color{206, 182, 178, 255},
             resourceLine.str());

    std::ostringstream statusLine;
    statusLine << "Grafts: " << resources.grafts << "   Salvaged: " << resources.salvaged
               << "   Elite in: " << std::max(0, runState.eliteCountdown) << "   Stability "
               << FormatNumber(stability.demand) << "/" << FormatNumber(stability.capacity);
    if (paused_) {
        statusLine << "   [PAUSED]";
    }
    if (runFailed_) {
        statusLine << "   [RUN FAILED]";
    }
    DrawText(
        renderer_, panel.x + 16.0F, panel.y + 50.0F,
        stability.overload > 0.0F ? SDL_Color{214, 108, 98, 255} : SDL_Color{154, 196, 142, 255},
        statusLine.str());
}

void PrototypeGame::RenderPlayerPanel(const SDL_FRect& panel) const {
    FillRect(renderer_, panel, SDL_Color{28, 24, 30, 255});
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
                  << FormatNumber(stability.capacity);
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
    FillRect(renderer_, panel, SDL_Color{30, 22, 26, 255});
    DrawRect(renderer_, panel, SDL_Color{102, 80, 90, 255});
    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F, SDL_Color{232, 216, 216, 255},
             "Approach lane");

    const SDL_FRect laneRect{panel.x + 16.0F, panel.y + 38.0F, panel.w - 32.0F, 210.0F};
    RenderLane(laneRect);

    if (currentEnemy_ != entt::null && registry_.valid(currentEnemy_)) {
        const Name& name = registry_.get<Name>(currentEnemy_);
        const Health& health = registry_.get<Health>(currentEnemy_);
        const Enemy& enemy = registry_.get<Enemy>(currentEnemy_);

        DrawText(renderer_, panel.x + 18.0F, panel.y + 262.0F, SDL_Color{220, 206, 194, 255},
                 name.value);
        DrawBar(renderer_, SDL_FRect{panel.x + 18.0F, panel.y + 280.0F, 260.0F, 12.0F},
                health.current / std::max(1.0F, health.maximum), SDL_Color{182, 86, 96, 255},
                SDL_Color{84, 38, 48, 255});

        std::ostringstream line;
        line << "Tier " << enemy.tier << "  "
             << (combatJoined_ ? "CONTACT - auto-combat active" : "ADVANCING FROM THE RIGHT");
        if (enemy.elite) {
            line << "  [ELITE]";
        }
        DrawText(renderer_, panel.x + 18.0F, panel.y + 298.0F, SDL_Color{196, 188, 182, 255},
                 line.str());
    } else if (currentCorpse_ != entt::null && registry_.valid(currentCorpse_)) {
        const Corpse& corpse = registry_.get<Corpse>(currentCorpse_);
        DrawText(renderer_, panel.x + 18.0F, panel.y + 262.0F, SDL_Color{208, 204, 194, 255},
                 registry_.get<Name>(currentCorpse_).value);
        DrawBar(renderer_, SDL_FRect{panel.x + 18.0F, panel.y + 280.0F, 260.0F, 12.0F},
                1.0F - corpse.harvestTimer / std::max(0.01F, corpse.maxHarvestTimer),
                SDL_Color{112, 172, 96, 255}, SDL_Color{50, 74, 46, 255});
        DrawText(renderer_, panel.x + 18.0F, panel.y + 298.0F, SDL_Color{180, 196, 162, 255},
                 "Auto-harvesting the corpse for resources and salvage.");
    } else if (runFailed_) {
        DrawText(renderer_, panel.x + 18.0F, panel.y + 262.0F, SDL_Color{222, 120, 108, 255},
                 "Run failure: the homunculus collapsed at the contact line.");
        DrawText(renderer_, panel.x + 18.0F, panel.y + 284.0F, SDL_Color{206, 196, 188, 255},
                 "Press R to restart or E to dissolve the remains into banked essence.");
    } else {
        DrawText(renderer_, panel.x + 18.0F, panel.y + 262.0F, SDL_Color{208, 198, 188, 255},
                 "The player keeps walking in place while the next enemy approaches.");
        DrawText(renderer_, panel.x + 18.0F, panel.y + 284.0F, SDL_Color{176, 170, 164, 255},
                 "Combat begins automatically once the enemy reaches the graft line.");
    }
}

void PrototypeGame::RenderLane(const SDL_FRect& laneRect) const {
    const float groundY = laneRect.y + kLaneGroundOffsetY;

    FillRect(renderer_, laneRect, SDL_Color{34, 24, 30, 255});
    FillRect(renderer_,
             SDL_FRect{laneRect.x, laneRect.y + 18.0F, laneRect.w, groundY - laneRect.y - 18.0F},
             SDL_Color{48, 28, 36, 255});
    FillRect(renderer_,
             SDL_FRect{laneRect.x, groundY, laneRect.w, laneRect.h - (groundY - laneRect.y)},
             SDL_Color{64, 44, 40, 255});

    for (int stripe = 0; stripe < 7; ++stripe) {
        const float stripeX = laneRect.x + 18.0F + static_cast<float>(stripe) * 98.0F;
        FillRect(renderer_, SDL_FRect{stripeX, groundY + 22.0F, 44.0F, 4.0F},
                 SDL_Color{96, 70, 64, 255});
    }

    DrawRect(renderer_, laneRect, SDL_Color{136, 112, 106, 255});
    DrawRect(renderer_,
             SDL_FRect{laneRect.x + kLaneEnemyContactX - 10.0F, laneRect.y + 22.0F, 12.0F,
                       laneRect.h - 40.0F},
             SDL_Color{146, 72, 78, 255});

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
             "Walk animation loops while the enemy advances from the right.");

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
        RenderEnemyFigure(renderer_, enemyRect, enemyTemplate.color, enemy.elite);
        DrawBar(renderer_,
                SDL_FRect{enemyRect.x + 8.0F, enemyRect.y - 18.0F, enemyRect.w - 16.0F, 10.0F},
                enemyHealth.current / std::max(1.0F, enemyHealth.maximum),
                SDL_Color{184, 94, 98, 255}, SDL_Color{82, 38, 44, 255});

        DrawText(renderer_, enemyRect.x + 18.0F, enemyRect.y + enemyRect.h + 8.0F,
                 SDL_Color{220, 214, 204, 255}, combatJoined_ ? "ENGAGED" : "APPROACHING");
    } else if (currentCorpse_ != entt::null && registry_.valid(currentCorpse_)) {
        const Corpse& corpse = registry_.get<Corpse>(currentCorpse_);
        const EnemyTemplate enemyTemplate = EnemyTemplateFor(corpse.source);
        const SDL_FRect corpseRect{laneRect.x + corpseLaneX_, groundY - 48.0F, kEnemyRenderWidth,
                                   52.0F};
        RenderCorpsePile(renderer_, corpseRect, enemyTemplate.color);
        DrawText(renderer_, corpseRect.x + 24.0F, corpseRect.y - 18.0F,
                 SDL_Color{198, 212, 188, 255}, "HARVEST");
    } else {
        DrawText(renderer_, laneRect.x + kLaneEnemySpawnX - 66.0F, laneRect.y + 34.0F,
                 SDL_Color{182, 182, 192, 255}, "ENTRY");
    }
}

void PrototypeGame::RenderEventLog(const SDL_FRect& panel) const {
    FillRect(renderer_, panel, SDL_Color{24, 22, 24, 255});
    DrawRect(renderer_, panel, SDL_Color{90, 82, 88, 255});
    DrawText(renderer_, panel.x + 16.0F, panel.y + 12.0F, SDL_Color{224, 214, 206, 255},
             "Recent events");

    float y = panel.y + 34.0F;
    int lineIndex = 0;
    for (const std::string& line : eventLog_) {
        const SDL_Color color =
            (lineIndex == 0) ? SDL_Color{220, 210, 198, 255} : SDL_Color{166, 166, 166, 255};
        DrawText(renderer_, panel.x + 16.0F, y, color, line);
        y += 20.0F;
        ++lineIndex;
    }
}
