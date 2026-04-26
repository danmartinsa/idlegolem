#Copilot Instructions

## Build commands

```bash
cmake -S . -B build
cmake --build build
```

## High-level architecture

- `src/main.cpp` is a thin entrypoint that wires the concrete game into the engine layer.
- `src/engine/` owns the SDL3 application lifecycle: init, window and renderer creation, the event loop, timing, presentation, and teardown.
- `src/game/` owns Fleshgolem-specific gameplay, content, rendering, and audio.
- `CMakeLists.txt` builds a single executable named `idlegolem`, requires C++20, and links against SDL3 and EnTT. It prefers installed SDL3 3.4.4 / EnTT 3.16.0 packages and falls back to `FetchContent` when needed.
- Game structure and feature intent are documented outside the runtime code. `GDD.md` is the more detailed source for the incremental-idle RPG loop, body-part system, automation, and meta-progression. `game_design_document.md` covers the same game at a higher level with some different terminology. When adding gameplay systems, use the docs as product guidance and keep terminology consistent instead of inventing a third interpretation.

## Key conventions

- Keep SDL startup and shutdown explicit. The existing code checks each SDL call, logs failures with `SDL_Log`, and tears down only the resources that were successfully created.
- `SDL_MAIN_HANDLED` is defined in `src/main.cpp`; keep that pattern unless entrypoint handling is intentionally redesigned.
- File-local constants live in an anonymous namespace and use the `kName` constant style (`kWindowWidth`, `kWindowTitle`).
- The current gameplay loop is expected to remain UI-driven / auto-battle oriented rather than twitch-action. New systems should fit the design-doc direction: scavenging, grafting body parts, automation unlocks, and run/meta progression.
- There are two overlapping design docs with slightly different names for stats, currencies, and systems. Prefer extending one established term set within a change instead of mixing vocabulary from both docs in the same feature.
