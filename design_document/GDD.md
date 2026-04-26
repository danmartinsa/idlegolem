# Fleshgolem - Game Design Document

## 1. Document Purpose

This document is the unified game design reference for **Fleshgolem**. It merges the high-level pitch and named world details from `GDD.md` with the broader systems design from `game_design_document.md`, then resolves conflicting terminology into a single canon.

The goal is to define:

- the player fantasy
- the core progression loops
- the major gameplay systems
- the world and content structure
- the terminology future implementation should use

---

## 2. Game Summary

**Fleshgolem** is a dark-fantasy incremental idle RPG about a fragile homunculus that scavenges, harvests, and grafts flesh into a stronger body. The player begins each run as an incomplete artificial creature and gradually becomes more capable by defeating enemies, collecting anatomical loot, assembling synergistic body parts, unlocking automation, and banking permanent meta progression between runs.

The game is primarily **UI-driven and systems-driven**, not twitch-action. Combat is mostly automated, while the real skill comes from preparation, build specialization, loot triage, graft decisions, automation rules, and prestige timing.

The core fantasy is:

**You build yourself out of the ruined world and the enemies you defeat.**

---

## 3. Design Pillars

1. **You are what you stitch together.**  
   Body parts are the central equipment, build, and identity system.

2. **Every encounter must feed progression.**  
   Combat, scavenging, and exploration should always give parts, resources, knowledge, or meta advancement.

3. **Idle play matters, but active decisions outperform it.**  
   The player can automate repetition, but buildcraft, timing, and targeted farming should produce better results.

4. **Failure still advances the account.**  
   Runs can end in death, retreat, or deliberate dissolution, but permanent systems continue to move forward.

5. **Macabre presentation supports readable systems.**  
   The world is grotesque, but stats, loot, and decision-making must remain legible and satisfying.

6. **Build identity should emerge early.**  
   Even in the early game, players should feel pulled toward recognizable body archetypes and stat priorities.

---

## 4. Player Fantasy and Emotional Arc

The player is not a hero. They are a failed creation trying to become complete, powerful, and self-determined.

The emotional arc should move through:

1. **Weakness** - a shambling meat construct with poor stats and unstable parts
2. **Adaptation** - replacing weak anatomy with useful grafts
3. **Specialization** - committing to a combat style, mutation path, or farming strategy
4. **Dominance** - defeating factions and bosses for defining body parts
5. **Transcendence** - becoming something beyond a disposable homunculus

The game should constantly ask:

- Are you restoring yourself into a perfected being?
- Or are you becoming a monstrous engine of survival?

---

## 5. Genre, Camera, and Session Structure

- **Genre:** Incremental idle RPG / body-horror evolution simulator
- **Presentation:** Menu-driven and UI-driven with stylized combat feedback
- **Play pattern:** Short active bursts plus longer idle farming windows
- **Session expectation:** The player can log in, make meaningful build decisions, then leave systems running at reduced efficiency
- **Core tone:** grim, fleshy, occult, strategic, strangely triumphant

---

## 6. Canonical Terminology

The two earlier design docs use overlapping but inconsistent names. This document standardizes the main vocabulary.

### 6.1 Core Terms

- **Body Parts:** equippable anatomy pieces harvested, crafted, or mutated
- **Grafting:** equipping, replacing, repairing, or fusing anatomy
- **Decay:** loss of part integrity over time or use
- **Stability:** how well a body can support powerful or incompatible anatomy
- **Mutation:** high-risk systems that trade safety for stronger effects
- **Automation:** unlocked behavior rules for combat, scavenging, and loot handling
- **Dissolve:** the main prestige/reset action

### 6.2 Resource and Currency Canon

To preserve good ideas from both source docs, use these canonical resources:

- **Biomass** - common crafting and surgical material gathered from combat and scavenging
- **Bone Meal** - run-based experience resource used for attribute growth and some upgrades
- **Raw Meat** - low-tier consumable salvage used in fusion, repair, and preservation recipes
- **Essence** - core meta currency earned from meaningful run progress
- **Research** - knowledge currency from discovering anatomy, enemies, and labs
- **Memory Shards** - boss and milestone progression currency
- **Catalyst Ichor** - rare late-game prestige material for transformational upgrades
- **Primordial Essence** - special Essence payout awarded when dissolving a run

### 6.3 Attribute Naming Convention

Use clear stat names in UI, with optional biological flavor names in tooltips or flavor text:

| Gameplay Name | Flavor Alias | Main Use |
| :--- | :--- | :--- |
| **Might** | Sinew | physical damage, carry weight, heavy graft compatibility |
| **Vitality** | Grafting | health, healing, decay resistance |
| **Agility** | Reflex | attack speed, dodge, initiative |
| **Reason** | Neural | automation quality, graft efficiency, skill capacity |
| **Instinct** | Mutation | crit rate, loot sensing, ambush resistance |
| **Corruption** | Forbidden Strain | unstable power, mutation scaling, risk exposure |

This keeps the broad system readability from `game_design_document.md` while preserving the flavor identity from `GDD.md`.

---

## 7. Core Gameplay Loops

### 7.1 Moment-to-Moment Loop

1. Choose a zone, hunt, or scavenging task
2. Enter auto-combat or automated harvesting
3. Gain body parts, Biomass, Raw Meat, Bone Meal, and occasional rare resources
4. Compare new parts to existing anatomy
5. Graft upgrades, preserve valuable parts, or salvage bad loot
6. Push deeper or pivot to a safer farming route

### 7.2 Run Loop

1. Start as a weak homunculus template
2. Stabilize the body with early slots and reliable resources
3. Define a build through part synergies, stats, and mutations
4. Unlock or improve automation rules
5. Farm elites and bosses for signature anatomy
6. Decide whether to risk another push or extract value
7. End through death, escape, or Dissolve

### 7.3 Meta Loop

1. Convert run progress into Essence, Research, and long-term unlocks
2. Improve starting templates, drop floors, and crafting options
3. Unlock new zones, enemy factions, and prestige layers
4. Gain stronger automation and deeper mutation systems
5. Begin the next run with more strategic options

---

## 8. The Body System

The body is the primary equipment sheet, build engine, and visual identity system.

### 8.1 Major Body Slots

- Head
- Torso
- Left Arm
- Right Arm
- Left Leg
- Right Leg
- Heart
- Lungs
- Stomach
- Spine
- Skin
- Blood

### 8.2 Slot Functions

- **Head:** sensory bonuses, crit support, scouting, skill capacity
- **Torso:** max health, organ capacity, defensive baseline
- **Arms:** attack pattern, damage type, attack speed modifiers
- **Legs:** initiative, evasion, movement between zones, retreat success
- **Heart:** sustain, rage effects, health scaling, lifesteal interactions
- **Lungs:** stamina, poison handling, decay interaction, breath-based passives
- **Stomach:** salvage yield, consumable efficiency, corruption digestion
- **Spine:** coordination, combo support, automation complexity limits
- **Skin:** armor, resistances, environmental defenses
- **Blood:** healing, rage states, status propagation, instability modifiers

### 8.3 Part Properties

Each part may contain:

- flat stat bonuses
- percentage modifiers
- active skill grants
- passive traits
- set or species synergies
- mutation tags
- decay rate
- weight
- stability demand
- rarity tier
- salvage value
- faction or biome origin

### 8.4 Part Quality Tiers

- Common
- Uncommon
- Rare
- Elite
- Monstrous
- Mythic

Higher-tier parts should feel transformative, not just numerically larger. They should often introduce strong traits, major tradeoffs, or unusual build hooks.

### 8.5 Part Identity Examples

- **Bandit Arm:** fast strikes, low durability, bonus salvage chance
- **Wolf Leg:** dodge, initiative, chase/retreat bonuses
- **Knight Heart:** high Vitality, armor synergy, slow but reliable sustain
- **Necrotic Lung:** poison resistance and corruption scaling at the cost of healing
- **Alchemist Eye:** crit chance, loot quality sensing, lab-event bonuses
- **Dragon Bone Spine:** enormous Might scaling, high weight, strict stability requirement
- **Acid Gland:** poison application, corrosion traits, synergy with cult and fungal enemies

### 8.6 Freshness, Decay, and Preservation

Borrowing from `GDD.md`, parts should not be permanently perfect.

- Fresh parts have full power
- Decaying parts lose effectiveness over time or repeated use
- Some parts decay faster in hostile environments
- Preservation systems can slow, halt, or partially reverse decay
- Certain cursed or eldritch parts may become stronger as they rot, creating deliberate risk builds

Decay gives the loot game churn, makes inventory decisions matter, and supports the body-horror theme.

### 8.7 Stability and Rejection

The body cannot safely support unlimited power.

- Each advanced part has a **stability demand**
- The player has a **stability capacity** based on stats, upgrades, and body composition
- Exceeding safe limits causes penalties such as:
  - lower healing
  - increased decay
  - skill misfires
  - graft rejection
  - combat debuffs
  - catastrophic collapse in extreme cases

This system makes buildcraft about fit and structure, not just equipping the highest rarity part.

---

## 9. Attributes and Derived Stats

### 9.1 Primary Attributes

- **Might:** physical damage, carry weight, heavy graft compatibility
- **Vitality:** max health, healing received, decay resistance
- **Agility:** attack speed, dodge, initiative, pursuit/flee outcomes
- **Reason:** automation quality, crafting precision, graft success, active skill slots
- **Instinct:** crit rate, loot detection, ambush resistance, tracking
- **Corruption:** mutation strength, forbidden skill scaling, instability risk

### 9.2 Secondary Stats

- Health
- Stamina
- Armor
- Evasion
- Accuracy
- Crit Chance
- Crit Damage
- Life Steal
- Part Drop Rate
- Biomass Gain
- Essence Gain
- Bone Meal Gain
- Stability
- Decay Resistance
- Healing Efficiency
- Poison Resistance
- Bleed Chance
- Flee Success Rate

### 9.3 Build Archetypes

- **Berserker Graft Build:** high Might, rage organs, self-heal through damage
- **Evasion Scavenger:** Agility and Instinct focused, low durability, strong loot throughput
- **Tank Stitched Brute:** high Vitality, armor skin, heart sustain, stable slow combat
- **Poison/Decay Monster:** lungs, glands, blood synergy, attrition damage
- **Crit Hunter:** precision head and eye parts, ambush and execution bonuses
- **Mutation Caster:** high Reason plus Corruption, skill-heavy unstable playstyle

---

## 10. Combat System

Combat is mostly automated, but strategic control comes from preparation and rule-setting.

### 10.1 Combat Principles

- Encounters should resolve quickly and read clearly
- Gear and build decisions should be more important than mechanical execution
- The player should still make meaningful tactical choices through loadout and behavior rules

### 10.2 Combat Flow

1. Select zone and encounter type
2. Enter battle against a normal, elite, or boss enemy group
3. Auto-attacks and passives execute continuously
4. Active abilities trigger manually or via automation rules
5. Fight ends with loot, injury, retreat, or death

### 10.3 Player Inputs Outside Real-Time Action

- choose target priority
- enable or disable active skills
- set auto-cast conditions
- define retreat thresholds
- choose harvest focus after battle
- choose whether to preserve, salvage, or graft drops

### 10.4 Damage and Status Themes

Suggested damage/status families:

- physical
- bleed
- poison
- rot
- fire/alchemical burn
- occult/corruption

Different enemy factions and body archetypes should naturally lean toward these themes.

### 10.5 Encounter Types

- **Standard fights:** reliable farming and progression
- **Elite fights:** targeted high-value part drops and build checks
- **Boss fights:** major progression gates with signature rewards
- **Ambushes:** risk spikes tied to scouting and Instinct
- **Scavenge interruptions:** combat triggered while harvesting dangerous zones

---

## 11. Skills and Abilities

Skills should primarily come from body parts, mutations, and rare meta unlocks rather than from a traditional class tree.

### 11.1 Skill Sources

- body parts
- fused/twisted parts
- mutation lines
- boss reward anatomy
- permanent research unlocks

### 11.2 Skill Categories

- **Active skills:** triggered manually or automatically
- **Passive skills:** always on
- **Conditional traits:** activate under health, decay, or corruption thresholds
- **Behavioral skills:** modify automation logic and targeting

### 11.3 Example Skills

- **Bone Lance:** high single-target spike from arm or spine parts
- **Acid Vent:** poison burst from lung or gland organs
- **Predator Reflex:** dodge into counterattack from leg/head synergy
- **Emergency Stitching:** short survival heal triggered at low health
- **Carrion Beacon:** increases elite encounter chance in a zone
- **Rejection Burst:** turns instability into an offensive explosion

---

## 12. Scavenging and Harvesting

Scavenging is the non-combat backbone of progression and a major source of flavor.

### 12.1 Scavenge Activities

- search battlefields after combat
- harvest specific enemy parts
- raid labs, crypts, pits, and ritual sites
- dig through bone dumps and alchemical refuse
- send automated collectors to previously cleared locations

### 12.2 Reward Categories

- body parts
- Biomass
- Raw Meat
- alchemical reagents
- scrap tools
- preservation materials
- mutation catalysts
- lore fragments
- map keys
- hidden encounters

### 12.3 Harvest Decision Layer

After major encounters, the player may choose a focus:

- maximize valuable parts
- maximize crafting materials
- maximize Bone Meal
- minimize decay risk
- prioritize specific anatomy types

This supports targeted farming and lets enemy identity matter.

### 12.4 Risk and Reward

- safer zones offer stable but lower-tier anatomy
- cursed zones offer unstable, powerful parts
- some zones reward repeated farming efficiency
- others are best for short, high-risk pushes

---

## 13. Crafting, Surgery, and Fusion

Not every useful part should be a direct drop. The player needs systems to improve, combine, and specialize anatomy.

### 13.1 Core Crafting Functions

- repair damaged parts
- preserve fresh parts
- purge corruption
- fuse identical parts
- combine compatible anatomy into composite grafts
- distill rare resources into permanent materials
- mutate parts into forbidden variants

### 13.2 Signature Stations

- **Stitching Table:** equip, repair, remove, and compare anatomy
- **Vat of Rebirth:** fuse duplicate parts into twisted variants
- **Preservation Rack:** slow or halt decay
- **Alchemy Furnace:** convert Biomass, Raw Meat, and reagents into special materials
- **Memory Crucible:** convert boss resources into permanent unlocks

### 13.3 Twisted and Composite Parts

From the shorter `GDD.md`, the **Twisted Part** idea should remain core:

- created by combining matching parts
- receives one or more random enhanced modifiers
- may gain new instability or corruption risk

Composite parts from the larger doc can sit one tier above:

- combine parts from multiple species or roles
- create build-defining hybrids
- require higher Reason, Research, or rare crafting inputs

### 13.4 Surgery Constraints

- surgery costs time and materials
- some part families conflict
- powerful combinations may require stat thresholds
- removing cursed organs may cause penalties
- emergency swaps during a run should be limited or expensive

---

## 14. Mutation System

Mutation is the game’s high-risk, high-identity layer.

### 14.1 Mutation Goals

- create extreme build paths
- break baseline rules in interesting ways
- introduce meaningful long-term tradeoffs

### 14.2 Mutation Themes

- predatory ferality
- fungal infestation
- necrotic resilience
- dragonbone gigantism
- alchemical precision
- eldritch transcendence

### 14.3 Mutation Costs

- higher instability
- altered part compatibility
- increased decay or rejection risk
- story consequences and faction hostility

### 14.4 Example Mutation Outcomes

- gain poison immunity but lose conventional healing
- replace crit builds with guaranteed bleed effects
- transform retreat into an offensive maneuver
- sacrifice stable drops for monstrous rarity bias

---

## 15. Idle and Automation Systems

Automation should expand the player’s strategic reach without replacing decision-making.

### 15.1 Automation Unlock Philosophy

- early automation is crude and inefficient
- later automation becomes more precise through upgrades and Reason scaling
- the best results still come from active tuning and targeted runs

### 15.2 Automation Categories

- auto-scavenge chosen zones
- auto-salvage low-tier parts
- auto-preserve rare anatomy
- auto-equip upgrades within user-defined rules
- auto-cast skills
- auto-retreat at danger thresholds
- auto-harvest chosen enemy species
- auto-dissolve under defined conditions

### 15.3 Rule Examples

- salvage Common arms unless they match active build tags
- retreat if health drops below 20% and boss is above 50%
- preserve Rare organs automatically
- equip only parts that improve Instinct or Agility
- prioritize harvesting lungs and glands in toxic zones

Automation should feel like the player is programming a monster, not just toggling convenience features.

---

## 16. Run Failure, Extraction, and Prestige

Runs should end in a way that creates tension rather than simple exhaustion.

### 16.1 Run End States

- **Death:** lose the body, retain meta rewards based on progress
- **Escape/Extraction:** cash out safely with reduced bonus rewards
- **Dissolve:** intentionally sacrifice the body for high prestige conversion
- **Ascension Event:** rare late-game transition into a deeper progression layer

### 16.2 Dissolve as Prestige

The prestige action from `GDD.md` should remain a defining ritual.

When dissolving:

- the current body is destroyed
- run resources are converted into Primordial Essence and other meta gains
- certain milestone thresholds increase payout multipliers
- some rare parts may be permanently cataloged for future crafting or starting templates

### 16.3 Meta Upgrade Categories

- stronger starting bodies
- starting resource bonuses
- improved loot quality floor
- additional body slot capacity
- automation logic upgrades
- crafting and fusion unlocks
- mutation trees
- new zone access
- new prestige layers
- better preservation and stability tools

---

## 17. Progression Structure

### 17.1 Layer 1 - Basic Survival

- survive starter zones
- understand loot comparison
- unlock core body slots
- stabilize decay and injuries

### 17.2 Layer 2 - Build Specialization

- farm targeted anatomy
- lean into a stat identity
- unlock first automation tools
- begin elite hunting

### 17.3 Layer 3 - Mutation Mastery

- manage corruption-heavy builds
- access advanced surgery
- defeat mechanically distinct bosses
- create composite or twisted anatomy

### 17.4 Layer 4 - Prestige Expansion

- Dissolve powerful bodies efficiently
- unlock improved templates and passive systems
- access new enemy factions and body archetypes

### 17.5 Layer 5 - Transcendent Play

- pursue transformative body forms
- unlock late-game prestige resources
- challenge unique endgame zones and identity-defining bosses

---

## 18. World and Setting

The world is a corpse-ridden alchemical wasteland shaped by failed experiments, ritual warfare, and living machinery.

Zones should be distinct in:

- enemy families
- status effects
- anatomy types
- resource tables
- narrative flavor
- automation value

### 18.1 Core Factions and Threat Families

- scavengers and raiders
- wild beasts
- failed homunculi
- cult enforcers
- corpse-machines
- noble undead and stitched champions

### 18.2 Key Zones

This section combines named locations from both source docs.

1. **The Charnel Pits**  
   Tutorial region. Humanoid and rodent parts, basic decay lessons, early scavenging loops.

2. **Rot Farm**  
   Easy-to-mid zone full of farm-mutants and plentiful Biomass. Good for early preservation recipes.

3. **Bone Warrens**  
   Ambush-heavy tunnels focused on skeletal materials, scouting, and retreat logic.

4. **The Fungal Wastes**  
   Toxic environment with poison mechanics, fungal beasts, and mutation catalysts.

5. **Ash Chapel**  
   Cult-controlled territory with ritual enemies, corruption rewards, and lore fragments.

6. **Surgery Pits**  
   High-risk grafting zone where elite enemies drop advanced organs and composite anatomy components.

7. **The Dragon's Graveyard**  
   Late-game zone for massive bone, scale, and heavy-body builds with extreme stability demands.

8. **Royal Ossuary**  
   Endgame tomb-complex with corpse-knights, relic organs, and defensive anatomy.

9. **The Alchemist's Spire**  
   Deep endgame laboratory filled with failed experiments, eldritch parts, and story revelations.

---

## 19. Boss Philosophy

Bosses should be progression-defining build checks rather than oversized normal enemies.

### 19.1 Boss Goals

- reward strategic adaptation
- introduce unique anatomy
- unlock new systems or regions
- reinforce the identity of a zone

### 19.2 Boss Reward Types

- signature body parts
- unique mutations
- Memory Shards
- automation protocols
- progression keys
- major story revelations

### 19.3 Boss Design Principles

- each boss should challenge a different defensive assumption
- some bosses punish greed and poor retreat logic
- some should hard-counter a popular build path to encourage flexibility
- every major boss should change the player's strategic options afterward

---

## 20. Narrative Structure

Story should be light, atmospheric, and discovery-based.

### 20.1 Delivery Channels

- zone descriptions
- enemy flavor text
- recovered lab notes
- surgery records
- mutation logs
- creator memories
- milestone events
- boss aftermath scenes

### 20.2 Core Narrative Questions

- Who created the homunculus?
- Why is the world saturated with grafting horrors?
- Is perfection equivalent to humanity, monstrosity, or autonomy?
- Are your upgrades restoring identity or erasing it?

### 20.3 Narrative Arc

- early game: survival and confusion
- mid game: discovery of factions, labs, and body doctrines
- late game: confrontation with the creators, the purpose of Dissolve, and the meaning of the player’s final form

---

## 21. UX and Presentation Goals

### 21.1 UX Priorities

- the player should instantly see what changed after a graft
- loot comparison must be readable at a glance
- status effects and decay should be obvious
- automation outcomes must be trustworthy and transparent
- elite and boss rewards should feel special before the player even equips them

### 21.2 Visual Direction

- gritty 2D paper-doll or anatomical assembly presentation
- strong silhouette changes from major body swaps
- jars, straps, bone braces, occult machinery, and wet stitched textures
- palette centered on deep reds, sickly greens, bone white, blackened metal, and fungal yellow

### 21.3 Audio Direction

- low drones and industrial ambience
- wet surgical sounds and bone stress effects
- heavier impacts for brute builds
- ritual whispers and distortion for corruption-heavy systems

---

## 22. Content Expansion Opportunities

- faction reputation and alliances
- companion constructs or stitched retainers
- laboratory or hideout expansion
- challenge runs with anatomy restrictions
- seasonal mutation events
- asynchronous raid or hunt targets
- rare species catalogs
- body-template sharing or blueprint systems

---

## 23. MVP Scope

The first playable version should focus on the clearest expression of the fantasy, not full feature breadth.

### 23.1 MVP Features

- 3 to 5 zones
- basic auto-combat loop
- 6 to 8 body slots
- core attributes and derived stat formulas
- part drops and body equipping
- simple decay and preservation
- basic fusion or twisted-part crafting
- one automation layer
- one meta currency track
- several elites
- at least 2 bosses

### 23.2 MVP Success Conditions

The MVP works if players feel:

- each run improves later runs
- body parts create meaningful build choices
- active play beats passive farming
- grotesque presentation reinforces mechanics
- collecting better anatomy is the reason to keep playing

---

## 24. Comparison Summary of the Source Documents

### 24.1 What `GDD.md` Contributed Most Strongly

- strong short-form pitch
- named currencies and prestige flavor
- Freshness/decay framing
- named zones like The Charnel Pits and The Alchemist's Spire
- visual direction around paper-doll character changes

### 24.2 What `game_design_document.md` Contributed Most Strongly

- design pillars
- run loop and meta loop framing
- expanded body slot model
- automation, scavenging, crafting, and mutation structure
- progression layers and content philosophy

### 24.3 Main Conflicts Resolved Here

- stat names were unified into readable gameplay names with biological aliases
- resource naming was expanded instead of choosing one document exclusively
- world zones from both docs were combined into a broader campaign map
- the lighter pitch and the heavier systems design were merged into one canonical reference

---

## 25. Final Vision Statement

**Fleshgolem** should feel like a game where every corpse is a possibility, every graft is a strategic commitment, and every failed run leaves behind the knowledge needed to build a more horrifying and more perfect body next time.
