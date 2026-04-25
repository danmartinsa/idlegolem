# Copilot Instructions

## Build commands

```bash
cmake -S . -B build
cmake --build build
```

## High-level architecture

- The current runtime is a very small SDL3 prototype in `src/main.cpp`. It owns the full executable lifecycle itself: `SDL_Init`, window creation, renderer creation, the event/render loop, and explicit teardown.
- `CMakeLists.txt` builds a single executable named `idlegolem`, requires C++20, and links against SDL3. It prefers a system SDL3 package first and falls back to fetching SDL 3.4.0 with `FetchContent`.
- Game structure and feature intent are documented outside the runtime code. `GDD.md` is the more detailed source for the incremental-idle RPG loop, body-part system, automation, and meta-progression. `game_design_document.md` covers the same game at a higher level with some different terminology. When adding gameplay systems, use the docs as product guidance and keep terminology consistent instead of inventing a third interpretation.

## Key conventions

- Keep SDL startup and shutdown explicit. The existing code checks each SDL call, logs failures with `SDL_Log`, and tears down only the resources that were successfully created.
- `SDL_MAIN_HANDLED` is defined in `src/main.cpp`; keep that pattern unless entrypoint handling is intentionally redesigned.
- File-local constants live in an anonymous namespace and use the `kName` constant style (`kWindowWidth`, `kWindowTitle`).
- The current gameplay loop is expected to remain UI-driven / auto-battle oriented rather than twitch-action. New systems should fit the design-doc direction: scavenging, grafting body parts, automation unlocks, and run/meta progression.
- There are two overlapping design docs with slightly different names for stats, currencies, and systems. Prefer extending one established term set within a change instead of mixing vocabulary from both docs in the same feature.
