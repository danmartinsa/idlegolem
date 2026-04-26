# idlegolem

**idlegolem** is a C++20 SDL3 + EnTT demo for **Fleshgolem**, a dark-fantasy idle RPG about
building a stronger body from scavenged enemy parts.

The project has moved past the original prototype into a **gameplay-first demo** with:

- a title screen and run-to-run flow
- three themed zones with unique backdrops
- multiple enemy waves, elites, and bosses
- auto-combat, harvesting, decay, and body-part grafting
- explicit reward drafts with run upgrades
- dissolve-based meta progression through banked Essence
- generated BMP art assets and synthesized SDL audio

## Repository layout

- `src/` - application code, gameplay systems, and rendering
- `assets/` - generated sprite sheets, title art, and zone backdrops
- `design_document/GDD.md` - game design reference
- `design_document/ECS.md` - ECS architecture reference
- `CMakeLists.txt` - build configuration and dependency setup

## Dependencies

The demo uses:

- **SDL3**
- **EnTT**
- **CMake 3.24+**
- a **C++20** compiler

The build prefers installed SDL3/EnTT packages when available. If they are not installed, CMake
fetches them automatically.

## Build

```bash
cmake -S . -B build
cmake --build build
```

To force dependency fetching instead of using system packages:

```bash
cmake -S . -B build -DIDLEGOLEM_USE_SYSTEM_SDL3=OFF -DIDLEGOLEM_USE_SYSTEM_ENTT=OFF
cmake --build build
```

## Run

```bash
./build/idlegolem
```

## Controls

- `Enter` / `Space` on the title screen - start a run
- `Space` during a run - pause or resume
- `R` - restart the current run
- `E` - dissolve the current or failed run into banked Essence
- `N` - force the next encounter sooner
- `1`, `2`, `3` - choose a reward draft option
- `Left` / `Right` + `Enter` - alternate reward selection controls
- mouse clicks - title, pause, reward, and end-screen buttons

## Demo loop

1. Start in the title screen and begin a fresh homunculus run.
2. Push through zone waves as enemies approach from the right and auto-combat starts on contact.
3. Harvest corpses for resources and body parts, then auto-graft stronger anatomy.
4. Defeat elites and bosses to open reward drafts that shape the rest of the run.
5. Clear the three demo zones or dissolve early to bank Essence for stronger future starts.

## Editor support

The project exports `compile_commands.json` and links it into the repository root so editors such
as `clangd` can resolve SDL3 and EnTT includes after configuring the build once.
