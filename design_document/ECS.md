# Fleshgolem ECS Design Document

## 1. Purpose

This document defines a practical **Entity Component System (ECS)** model for **Fleshgolem**. It translates the game design into data-oriented gameplay architecture so implementation can separate:

- what game objects **are** (`entities`)
- what data they **carry** (`components`)
- what game logic **acts on them** (`systems`)

The intent is to support:

- auto-combat and background simulation
- highly modular body-part equipment
- loot-heavy progression
- layered status, decay, mutation, and automation rules
- clean separation between run state and meta progression

This document is not engine-specific. It should remain valid whether the project uses a custom ECS, EnTT, Flecs, or a bespoke data-oriented layer.

---

## 2. ECS Design Principles for Fleshgolem

### 2.1 Core Principles

1. **Composition over inheritance**  
   A scavenger, wolf, or homunculus should mostly be different combinations of components rather than separate hard-coded classes.

2. **Data drives behavior**  
   Combat roles, mutations, loot behavior, and automation should come from components and definitions rather than entity-specific branching.

3. **Runtime entities, static definitions**  
   Runtime entities should reference immutable content definitions for parts, skills, zones, enemies, and recipes.

4. **Run state and meta state stay separate**  
   Most gameplay entities belong to the current run. Persistent account progress should live in a separate progression layer, not in combat entities.

5. **Temporary gameplay events should also be representable in ECS**  
   Projectiles, status applications, loot drops, harvest opportunities, and timer-driven tasks can all be modeled as entities when it improves clarity.

6. **The body is a modular graph, not a flat stat blob**  
   The player’s anatomy should be represented through part entities and slot/equipment relationships so grafting, decay, and replacement stay flexible.

---

## 3. High-Level ECS World Model

The game benefits from thinking in terms of several logical worlds or domains, even if they are implemented in one registry:

### 3.1 Domain Layers

| Domain | Purpose | Typical Lifetime |
| :--- | :--- | :--- |
| **Meta World** | account progression, unlocks, templates, research | persistent |
| **Run World** | current expedition, player body, zone state, encounters | per run |
| **Combat Scene** | active encounter participants, effects, timers | per encounter |
| **UI Projection Layer** | inspectable views of entities for menus and paper-doll rendering | frame-to-frame |

### 3.2 Recommended Boundary

- **Meta World** stores persistent unlocks, research, templates, automation presets, and cataloged discoveries.
- **Run World** stores the current homunculus, inventory, current zone progression, unlocked run mechanics, and temporary upgrades.
- **Combat Scene** stores transient battle entities and effects derived from run entities.

This separation keeps prestige and account state from becoming tangled with per-frame combat logic.

---

## 4. What Counts as an Entity

An entity should exist when it has **identity plus variable state** that matters to systems.

In Fleshgolem, the main entity categories are:

1. **Actors** - player body, enemies, bosses, summoned constructs
2. **Anatomy Parts** - equipped or inventory body parts
3. **Combat Effects** - projectiles, spawned hazards, delayed triggers
4. **Loot Objects** - dropped parts, resources, reward bundles
5. **World Nodes** - zones, encounter nodes, stations, extraction points
6. **Tasks** - scavenging jobs, crafting jobs, preservation jobs, automation work items
7. **Meta Objects** - unlocked templates, research entries, upgrade nodes

Not everything must be an entity. Static content definitions, tables, and recipe data should usually remain plain data assets keyed by ID.

---

## 5. Static Definitions vs Runtime Entities

To avoid bloated entities, the game should distinguish between:

### 5.1 Static Definitions

These are content records, not ECS entities:

- body part definitions
- enemy archetype definitions
- zone definitions
- skill definitions
- mutation definitions
- loot tables
- recipe definitions
- status effect definitions
- automation rule templates

### 5.2 Runtime Entities

These are instantiated during play:

- the current player body
- a specific wolf enemy in an encounter
- a Rare Knight Heart with partial decay
- a poison cloud hazard currently active in combat
- an elite corpse pile waiting to be harvested
- an in-progress preservation task

Runtime entities should point to definitions using IDs such as:

- `PartDefinitionId`
- `EnemyArchetypeId`
- `SkillDefinitionId`
- `ZoneDefinitionId`

---

## 6. Entity Catalog

This section lists the main entity types that should exist in the game.

## 6.1 Core Actor Entities

### 6.1.1 Player Homunculus

The main actor entity for the current run.

Responsibilities:

- owns combat-facing stats
- references equipped body parts
- receives statuses, mutations, and automation rules
- tracks health, stamina, corruption, stability, and current combat state

### 6.1.2 Enemy Actor

A generic runtime entity used for all enemies.

Specialization comes from:

- archetype definition ID
- faction tags
- attack profiles
- loot profiles
- mutation tags
- boss flags

### 6.1.3 Boss Actor

Still an enemy actor, but typically distinguished by:

- boss tag component
- phase logic markers
- signature loot
- story trigger links

### 6.1.4 Summoned / Spawned Ally

Used for companion constructs, temporary flesh masses, or mutation-born helpers.

### 6.1.5 Corpse / Harvestable Remains

Created after combat when an actor dies and remains harvestable.

This is important because:

- loot can depend on the final state of the enemy
- harvesting is a separate interaction from killing
- corpse decay and salvage quality can change over time

## 6.2 Anatomy Entities

### 6.2.1 Body Part Entity

Every individual anatomical item should be a runtime entity.

Reasons:

- decay varies per item
- rarity rolls and modifiers vary per item
- graft compatibility varies per item
- inventory and equipment need item identity

Examples:

- Common Bandit Arm
- Rare Wolf Leg with poison resistance affix
- Twisted Knight Heart with unstable graft trait

### 6.2.2 Composite / Twisted Part Entity

Usually still a Body Part entity with additional components indicating fusion results, special modifiers, and corruption risk.

## 6.3 Encounter and World Entities

### 6.3.1 Zone Entity

Represents the currently active zone instance or a world-map node.

Useful when zones need runtime state such as:

- encounter exhaustion
- discovered sub-sites
- active hazards
- current automation assignment
- depletion or corruption state

### 6.3.2 Encounter Entity

Represents a combat or scavenging event.

Useful for grouping:

- participants
- rewards
- encounter modifiers
- ambush/boss flags
- timing and completion state

### 6.3.3 World Station Entity

Used for:

- Stitching Table
- Vat of Rebirth
- Preservation Rack
- Alchemy Furnace
- Memory Crucible

Stations may live as menu-level entities or plain data-backed interactables depending on how diegetic the UI becomes.

### 6.3.4 Extraction Point / Event Node

Represents escape options, boss gates, narrative triggers, and run transition points.

## 6.4 Loot and Reward Entities

### 6.4.1 Loot Drop Entity

Represents a dropped object in encounter results or on the field.

May either:

- directly reference a spawned item entity
- or represent a resource bundle waiting to be claimed

### 6.4.2 Resource Bundle Entity

Useful for aggregated rewards like Biomass, Bone Meal, Raw Meat, Essence fragments, and Research packets.

## 6.5 Task and Timer Entities

### 6.5.1 Crafting Task

Represents an in-progress recipe with:

- inputs
- remaining time
- output definition
- assigned station

### 6.5.2 Scavenging Task

Represents background automation in a zone.

### 6.5.3 Preservation Task

Represents a part currently being stabilized, repaired, or purified.

### 6.5.4 Delayed Trigger / Timer Entity

Useful for:

- delayed explosions
- timed buffs
- spawn waves
- retreat countdowns
- boss phase changes

## 6.6 Meta Progression Entities

These can be ECS entities or persistent records, but they map cleanly to entity-like objects:

- upgrade node
- research discovery
- unlocked body template
- mutation tree node
- codex entry
- automation preset

---

## 7. Component Catalog

The following sections define the component families recommended for the game.

## 7.1 Identity and Definition Components

### `NameComponent`

- display name
- optional subtitle
- localized text key if needed

### `DefinitionRefComponent`

- definition ID
- definition type
- optional version/content hash

Used by nearly every content-driven entity.

### `TagComponent`

Flexible tags for queries and content rules:

- `player`
- `enemy`
- `boss`
- `corpse`
- `harvestable`
- `organ`
- `arm-part`
- `fungal`
- `eldritch`

Prefer compact tag containers over many one-off boolean components for content classification.

---

## 7.2 Transform and Scene Components

These matter more if the game renders spatial combat or an explorable map.

### `TransformComponent`

- position
- rotation
- scale

### `SpriteComponent`

- sprite ID or atlas key
- layer
- tint
- flip state

### `AnimationComponent`

- animation state
- current frame
- elapsed time

### `FacingComponent`

- facing direction or side

### `ScreenAnchorComponent`

For UI-projected entities like paper-doll overlays or combat widgets.

---

## 7.3 Core Actor State Components

### `ActorComponent`

Marks an entity as a combat-capable actor.

May include:

- actor kind
- team/faction side
- participation flags

### `FactionComponent`

- faction ID
- hostility relationships
- species/family tags

### `LevelComponent`

- level
- elite rank
- scaling tier

### `HealthComponent`

- max health
- current health
- barrier/temporary health

### `StaminaComponent`

- max stamina
- current stamina
- regeneration state

### `VitalStateComponent`

- alive/dead/dying
- corpse spawned flag
- harvest availability

### `CombatStateComponent`

- idle
- preparing
- attacking
- recovering
- stunned
- retreating
- harvesting

### `InitiativeComponent`

- initiative score
- next action timer

### `ThreatComponent`

- aggro value
- target priority modifiers

---

## 7.4 Attribute and Derived Stat Components

### `PrimaryAttributesComponent`

- Might
- Vitality
- Agility
- Reason
- Instinct
- Corruption

### `DerivedStatsComponent`

- armor
- evasion
- accuracy
- crit chance
- crit damage
- life steal
- decay resistance
- poison resistance
- flee success
- stability capacity

### `ResourceGainModifiersComponent`

- Biomass gain multiplier
- Bone Meal gain multiplier
- Essence gain multiplier
- part drop rate modifier

### `DamageProfileComponent`

- base damage
- damage type weights
- penetration
- attack interval

### `DefenseProfileComponent`

- flat mitigation
- type resistances
- status resistances

---

## 7.5 Anatomy and Equipment Components

This is the heart of the game’s ECS model.

### `BodyComponent`

Attached to actors that own body slots.

Contains:

- slot map or slot references
- capacity rules
- body template ID

### `BodySlotComponent`

Usually attached to slot entities or represented as data inside the body.

Fields:

- slot type
- occupied item entity
- unlock state
- slot restrictions

### `EquippedToComponent`

Attached to a body part entity.

- owning actor entity
- slot type
- equip timestamp or sequence

### `InventoryComponent`

- owned item entities
- capacity
- sorting metadata

### `PartComponent`

Marks an entity as a body-part item.

Contains:

- part family
- slot type
- source species
- quality tier

### `PartAffixesComponent`

- randomized modifiers
- trait IDs
- set tags

### `DecayComponent`

- freshness value
- decay rate
- decay paused flag
- last updated time

### `StabilityDemandComponent`

- required stability
- instability contribution
- rejection risk weight

### `CompatibilityComponent`

- allowed body templates
- forbidden pairings
- stat thresholds
- mutation prerequisites

### `PreservationStateComponent`

- preserved flag
- preservation quality
- time until spoilage resumes

### `TwistedPartComponent`

- fusion source count
- twist modifier seed
- instability bonus

### `CompositePartComponent`

- source part IDs
- hybrid families
- special recipe provenance

---

## 7.6 Ability and Effect Components

### `AbilityLoadoutComponent`

- equipped active abilities
- passive ability references
- unlock slots

### `AbilitySourceComponent`

Explains where an ability came from:

- body part
- mutation
- boss reward
- research unlock

### `CooldownComponent`

- current cooldown
- max cooldown
- charges if applicable

### `CastRulesComponent`

- target rule
- trigger condition
- allowed auto-cast state

### `StatusContainerComponent`

- list of active statuses
- stack counts
- expiry timers

### `StatusEffectComponent`

Used on temporary status entities or embedded status instances.

- status type
- potency
- source entity
- duration
- tick interval

### `AuraComponent`

- radius or target filter
- applied status/effect
- ownership

### `ProcTriggerComponent`

- trigger event
- proc chance
- internal cooldown

---

## 7.7 Loot, Reward, and Harvest Components

### `LootTableComponent`

- loot table ID
- rarity modifiers
- guaranteed reward flags

### `DropOnDeathComponent`

- corpse creation rule
- resource payout rules
- boss reward binding

### `HarvestableComponent`

- harvest attempts remaining
- anatomy categories available
- decay during harvesting

### `HarvestProfileComponent`

- part extraction bias
- Biomass output
- Raw Meat output
- reagent chances

### `RewardBundleComponent`

- currencies
- item entities
- unlock references

### `ClaimableComponent`

- claim state
- owner restrictions
- expiration time

---

## 7.8 Mutation and Corruption Components

### `MutationSetComponent`

- active mutation IDs
- mutation tier
- mutation slots or branches

### `CorruptionComponent`

- current corruption
- corruption cap
- corruption gain rate
- corruption penalties

### `InstabilityComponent`

- current instability
- safe threshold
- failure outcomes

### `RejectionRiskComponent`

- rejection chance
- triggers
- mitigation sources

These components support the game’s most dangerous build paths and should be queried by both combat and surgery systems.

---

## 7.9 AI and Automation Components

### `AITacticsComponent`

- behavior profile
- target selection rule
- preferred range/style
- retreat logic

### `AutomationControllerComponent`

Attached mainly to the player or run-level controller entity.

- enabled automation categories
- efficiency modifiers
- automation tier

### `AutomationRuleSetComponent`

- ordered rule list
- conditions
- priorities
- fallback behavior

### `AutoCastComponent`

- per ability trigger thresholds
- safety constraints

### `AutoLootComponent`

- keep/salvage/preserve rules
- rarity filters
- slot relevance checks

### `AutoScavengeComponent`

- target zone
- target species
- risk tolerance
- duration cadence

---

## 7.10 Crafting, Station, and Task Components

### `RecipeQueueComponent`

- queued recipe entries
- queue limit

### `CraftingTaskComponent`

- recipe ID
- input references
- start time
- completion time

### `StationComponent`

- station type
- unlocked services
- efficiency modifier

### `AssignedStationComponent`

- station entity reference

### `RepairableComponent`

- repair cost data
- repair progress

### `PurifiableComponent`

- corruption cleansing cost
- success rate modifiers

---

## 7.11 Zone, Encounter, and Progression Components

### `ZoneComponent`

- zone definition ID
- tier
- biome tags
- ambient hazards

### `ZoneRuntimeStateComponent`

- discovered state
- current threat level
- depletion level
- unlocked routes

### `EncounterComponent`

- encounter type
- difficulty rating
- source zone
- completion state

### `EncounterParticipantsComponent`

- attacker group
- defender group
- reserve spawns

### `EncounterModifiersComponent`

- elite modifier
- boss modifier
- ambush modifier
- weather/curse flags

### `ExtractionComponent`

- extraction type
- cost
- reward conversion factor

### `RunProgressComponent`

- current run age
- milestones reached
- bosses defeated
- extraction/dissolve availability

---

## 7.12 Narrative and UI Components

### `NarrativeTriggerComponent`

- trigger condition
- event ID
- one-shot/repeatable state

### `CodexEntryComponent`

- discovery category
- unlocked flag
- presentation priority

### `InspectComponent`

- inspect panel type
- UI summary fields

### `PaperDollVisualComponent`

- visual body part mapping
- mesh/sprite layer assignment
- damage/decay overlay state

This component is especially useful because the player’s body appearance should visibly change with grafts.

---

## 8. System Catalog

Systems should be responsible for behavior and state transitions, not for storing gameplay truth beyond short-lived caches.

## 8.1 Initialization and Content Systems

### `DefinitionLoadSystem`

Loads static content definitions and validates references.

### `EntitySpawnSystem`

Creates runtime entities from archetype/definition data.

### `RunBootstrapSystem`

Initializes a new run:

- starting body template
- starter inventory
- current zone
- run resource containers

### `EncounterSetupSystem`

Builds encounter entities and participant actors from zone and encounter data.

---

## 8.2 Stat and Composition Systems

### `BodyAssemblySystem`

Resolves what parts are equipped to each slot and whether slot constraints are satisfied.

### `AttributeAggregationSystem`

Combines:

- base actor values
- body part bonuses
- mutation modifiers
- temporary statuses
- meta bonuses

Outputs current primary attributes.

### `DerivedStatSystem`

Builds secondary combat and utility stats from primary attributes and modifiers.

### `StabilitySystem`

Calculates:

- current stability capacity
- total stability demand
- resulting instability penalties

### `CompatibilityValidationSystem`

Checks whether equipped or attempted grafts violate:

- slot rules
- mutation prerequisites
- family conflicts
- stat thresholds

---

## 8.3 Combat Systems

### `InitiativeSystem`

Advances action readiness and decides who can act next.

### `TargetSelectionSystem`

Chooses valid targets based on:

- AI rules
- automation rules
- taunt/threat
- encounter state

### `AttackResolutionSystem`

Handles basic attacks:

- hit checks
- crit checks
- mitigation
- damage application
- on-hit triggers

### `AbilityExecutionSystem`

Resolves active abilities and special attacks.

### `StatusApplicationSystem`

Applies bleed, poison, rot, corruption, buffs, and debuffs.

### `StatusTickSystem`

Processes periodic effects and expiry.

### `DeathResolutionSystem`

Transitions actors from alive to dead and spawns corpses, rewards, and triggers.

### `RetreatSystem`

Handles flee attempts, retreat success, escape penalties, and associated automation.

### `BossPhaseSystem`

For bosses with phase thresholds, new attack sets, or scripted transitions.

---

## 8.4 Loot, Corpse, and Harvest Systems

### `LootGenerationSystem`

Creates reward bundles and item entities based on enemy archetype, zone, rarity modifiers, and harvest choices.

### `CorpseSpawnSystem`

Spawns harvestable remains when appropriate.

### `HarvestSystem`

Converts corpses into:

- body parts
- Biomass
- Raw Meat
- reagents
- special organs

### `LootClaimSystem`

Moves rewards into inventory or resource storage.

### `LootFilterSystem`

Applies automation rules to salvage, preserve, equip, or discard loot.

---

## 8.5 Inventory, Grafting, and Item Systems

### `InventorySystem`

Owns item transfers, stacking policies for resources, and inventory capacity checks.

### `EquipSystem`

Moves a part from inventory into a valid body slot.

### `UnequipSystem`

Removes a part and updates dependent stats or slot validity.

### `PartDecaySystem`

Advances decay over time, combat usage, or environmental exposure.

### `PreservationSystem`

Slows or pauses decay using preservation tasks, recipes, or station services.

### `RepairSystem`

Restores damaged parts at material and time cost.

### `FusionSystem`

Creates Twisted Parts from identical or matching inputs.

### `CompositeGraftSystem`

Builds advanced hybrid anatomy from recipes and prerequisites.

### `SalvageSystem`

Breaks parts into materials, possibly influenced by part tier and preservation state.

---

## 8.6 Mutation and Risk Systems

### `CorruptionAccumulationSystem`

Adds or removes corruption from combat, zones, skills, and parts.

### `MutationUnlockSystem`

Grants mutation options from resources, story gates, or boss kills.

### `MutationApplicationSystem`

Activates mutation bonuses and penalties on eligible actors.

### `RejectionSystem`

Evaluates instability and incompatibility failures such as:

- stat penalties
- spontaneous debuffs
- forced unequip
- self-damage
- collapse events

---

## 8.7 Automation Systems

### `AutomationRuleEvaluationSystem`

Evaluates player automation rules in priority order.

### `AutoCastSystem`

Triggers active abilities when rule conditions match.

### `AutoEquipSystem`

Equips qualifying upgrades according to player rules.

### `AutoScavengeSystem`

Runs background scavenging jobs and generates periodic outcomes.

### `AutoRetreatSystem`

Monitors encounter danger and initiates retreat logic.

### `AutoDissolveSystem`

Late-game automation for prestige-driven farming loops.

Automation systems should not duplicate gameplay logic. They should invoke the same equip, loot, harvest, and run-end systems as manual actions.

---

## 8.8 World, Zone, and Encounter Systems

### `ZoneSelectionSystem`

Handles entering a zone and setting active zone state.

### `EncounterGenerationSystem`

Builds standard, elite, ambush, and boss encounters from zone data.

### `ZoneHazardSystem`

Applies environmental effects such as poison fog, rot, heat, or corruption pressure.

### `NarrativeEventSystem`

Triggers discoveries, logs, zone flavor, and milestone scenes.

### `ExtractionSystem`

Handles safe exit, reward conversion, and run termination transitions.

### `DissolveSystem`

Converts run output into Primordial Essence and other persistent rewards.

---

## 8.9 Meta Progression Systems

### `MetaRewardSystem`

Processes run-end output into persistent account gains.

### `ResearchSystem`

Unlocks codex entries, recipes, or systemic upgrades.

### `UpgradeTreeSystem`

Applies persistent bonuses from DNA tree, mutation tree, or account unlocks.

### `TemplateUnlockSystem`

Unlocks new starting bodies, slot capacity, or build seeds.

### `CatalogSystem`

Tracks discovered enemy species, body parts, and bosses.

---

## 8.10 Presentation Systems

### `PaperDollRenderSystem`

Projects equipped parts into a visual body.

### `CombatPresentationSystem`

Displays attacks, impacts, status indicators, and death events.

### `LootComparisonUISystem`

Builds side-by-side comparison views from item and body data.

### `InspectionSystem`

Generates readable UI summaries for actors, parts, zones, and stations.

---

## 9. Recommended Update Order

To keep behavior deterministic, the main gameplay loop should process systems in a consistent order.

### 9.1 Suggested Frame / Tick Order

1. input and UI intent collection
2. automation rule evaluation
3. manual command application
4. zone and encounter updates
5. body/equipment validation
6. attribute and derived stat recomputation
7. initiative and target selection
8. attack and ability execution
9. status ticks and environmental hazards
10. death resolution and corpse spawning
11. loot generation and harvest resolution
12. decay, repair, and preservation progression
13. crafting and task progression
14. narrative and codex triggers
15. presentation sync

For coarse idle simulation, the same order can be processed at larger time steps.

---

## 10. Suggested Archetypes

Archetypes here mean common component bundles.

## 10.1 Player Actor Archetype

Typical components:

- `ActorComponent`
- `FactionComponent`
- `LevelComponent`
- `HealthComponent`
- `StaminaComponent`
- `PrimaryAttributesComponent`
- `DerivedStatsComponent`
- `BodyComponent`
- `InventoryComponent`
- `AbilityLoadoutComponent`
- `StatusContainerComponent`
- `MutationSetComponent`
- `CorruptionComponent`
- `InstabilityComponent`
- `AutomationControllerComponent`
- `AutomationRuleSetComponent`
- `RunProgressComponent`
- `PaperDollVisualComponent`

## 10.2 Standard Enemy Archetype

- `ActorComponent`
- `FactionComponent`
- `LevelComponent`
- `HealthComponent`
- `PrimaryAttributesComponent`
- `DerivedStatsComponent`
- `DamageProfileComponent`
- `DefenseProfileComponent`
- `AITacticsComponent`
- `LootTableComponent`
- `DropOnDeathComponent`

## 10.3 Boss Archetype

Standard enemy components plus:

- `TagComponent` with `boss`
- `NarrativeTriggerComponent`
- `EncounterModifiersComponent`
- `MutationSetComponent`
- optional phase/timer components

## 10.4 Body Part Item Archetype

- `PartComponent`
- `DefinitionRefComponent`
- `PartAffixesComponent`
- `DecayComponent`
- `StabilityDemandComponent`
- `CompatibilityComponent`
- optional `TwistedPartComponent`
- optional `CompositePartComponent`
- optional `PreservationStateComponent`

## 10.5 Corpse Archetype

- `TagComponent` with `corpse`, `harvestable`
- `HarvestableComponent`
- `HarvestProfileComponent`
- `DecayComponent`
- `ClaimableComponent`

## 10.6 Station Archetype

- `StationComponent`
- `NameComponent`
- `InspectComponent`
- optional `RecipeQueueComponent`

---

## 11. MVP ECS Slice

The full ECS model is broad. The initial playable version should start with a smaller subset.

### 11.1 MVP Entities

- player homunculus
- standard enemy
- boss enemy
- body part item
- corpse
- zone
- encounter
- loot reward bundle

### 11.2 MVP Components

- identity/definition refs
- actor state
- health
- primary and derived stats
- body slots
- inventory
- part data
- decay
- loot tables
- harvestable
- status container
- zone and encounter state

### 11.3 MVP Systems

- run bootstrap
- encounter generation
- body assembly
- stat aggregation
- attack resolution
- status application/tick
- death resolution
- loot generation
- harvest
- equip/unequip
- decay
- basic preservation
- dissolve/meta reward conversion

This subset is enough to prove the game’s core fantasy before adding advanced automation, mutation trees, composite grafting, and richer narrative systems.

---

## 12. Important Modeling Decisions

### 12.1 Why Body Parts Should Be Entities

Because they carry unique runtime state:

- freshness/decay
- affixes
- rarity
- compatibility
- source origin
- preservation status

If parts were plain stat rows, the game would lose much of its progression depth.

### 12.2 Why Corpses Should Usually Be Entities

Harvesting is a second-stage interaction after combat. A corpse needs identity because it may:

- decay
- be harvested differently depending on player choice
- generate delayed events
- carry story or boss-specific rewards

### 12.3 Why Automation Should Be Components Plus Systems

Automation is not separate gameplay. It is a second control layer over the same verbs:

- fight
- loot
- preserve
- equip
- harvest
- retreat
- dissolve

That means automation should evaluate rules, then call the same systems as manual actions.

### 12.4 Why Zones May Need Runtime Entities

Even in a menu-driven game, zones can have mutable state:

- threat levels
- depletion
- unlocked sub-sites
- automation assignments
- temporary hazards

This makes them good ECS candidates if the world simulation grows deeper.

---

## 13. Risks and Anti-Patterns

### 13.1 Do Not Store Derived Stats in Too Many Places

If multiple components each keep their own "current damage" or "total armor" copy, bugs will appear quickly. Prefer a single authoritative derived stat output.

### 13.2 Do Not Hard-Code Enemy Classes for Every Species

Scavengers, fungal beasts, and corpse-machines should be content variants, not bespoke inheritance trees.

### 13.3 Do Not Mix Meta Progress Into Combat Entities

Persistent upgrades should be applied through systems at run start or stat aggregation time, not manually copied everywhere.

### 13.4 Do Not Let Automation Fork the Rules

Manual equip and auto-equip must share the same validation logic.

### 13.5 Do Not Flatten the Body into a Single Inventory Record

Fleshgolem’s identity depends on modular anatomy. Keep the slot and part model explicit.

---

## 14. Final ECS Vision

Fleshgolem’s ECS should make the game easy to expand by treating anatomy, enemies, loot, corruption, and automation as composable data rather than hand-scripted special cases.

At a healthy end state:

- new enemies are added mostly through content definitions and component bundles
- new body parts plug into existing stat, decay, and graft systems
- new mutations extend existing risk systems instead of bypassing them
- automation reuses manual gameplay rules
- the player body remains a visible, modular assembly of meaningful entities

The ECS succeeds if the game’s central fantasy remains true at the architecture level:

**a body built from parts, shaped by systems, and transformed by the consequences of every graft.**
