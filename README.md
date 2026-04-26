# idlegolem

**idlegolem** is a C++20 prototype for **Fleshgolem**, a dark-fantasy incremental idle RPG about building a stronger body from scavenged enemy parts. The current project is an SDL3 + EnTT vertical slice focused on the core loop: auto-combat, harvesting remains, grafting upgrades, part decay, stability pressure, and dissolving runs into long-term essence.

## Current prototype

The playable prototype includes:

- a single-window SDL3 presentation layer with a side-view encounter lane
- a generated player walk sprite sheet loaded directly by SDL3
- an EnTT-driven gameplay loop
- a homunculus with modular body slots
- enemies that enter from the right side of the screen and start combat on contact
- corpse harvesting for Biomass, Bone Meal, Essence, and body-part loot
- automatic grafting of stronger parts and salvaging of weaker ones
- part freshness/decay and stability overload
- run resets and dissolve-based meta progression

## Repository layout

- `src/` - application entry point and gameplay prototype code
- `assets/` - sprite and other runtime assets used by the prototype
- `design_document/GDD.md` - game design reference
- `design_document/ECS.md` - ECS architecture reference
- `CMakeLists.txt` - build configuration, SDL3/EnTT dependency setup

## Dependencies

The project uses:

- **SDL3**
- **EnTT**
- **CMake 3.24+**
- a **C++20** compiler

The build prefers installed SDL3/EnTT packages when available. If they are not installed, CMake fetches them automatically.

## Build

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

If you want to force the project to fetch dependencies instead of using system packages:

```bash
cmake -S . -B build -DIDLEGOLEM_USE_SYSTEM_SDL3=OFF -DIDLEGOLEM_USE_SYSTEM_ENTT=OFF
cmake --build build
```

## Run

After building, start the prototype with:

```bash
./build/idlegolem
```

## Prototype controls

- `Space` - pause or resume simulation
- `R` - restart the current run
- `E` - dissolve the run into banked essence
- `N` - force the next encounter sooner

## Editor support

The project exports `compile_commands.json` and links it into the repository root so editors such as `clangd` can resolve SDL3 and EnTT includes after configuring the build once.

## Design references

The implementation is guided by the documents in `design_document/`:

- `GDD.md` defines the game vision, systems, terminology, and prototype direction
- `ECS.md` defines the intended entity/component/system structure for the codebase
