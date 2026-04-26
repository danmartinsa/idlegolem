cmake_minimum_required(VERSION 3.24)

project(
    idlegolem
    VERSION 0.1.0
    DESCRIPTION
        "Incremental idle RPG about scavenging and fighting for body parts"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(
    IDLEGOLEM_USE_SYSTEM_SDL3
    "Prefer an installed SDL3 package before fetching it"
    ON
)
option(
    IDLEGOLEM_USE_SYSTEM_ENTT
    "Prefer an installed EnTT package before fetching it"
    ON
)

if(IDLEGOLEM_USE_SYSTEM_SDL3)
  find_package(SDL3 3.4.4 CONFIG QUIET)
endif()

if(IDLEGOLEM_USE_SYSTEM_ENTT)
  find_package(EnTT CONFIG QUIET)
endif()

if(NOT SDL3_FOUND OR NOT EnTT_FOUND)
  include(FetchContent)
endif()

if(NOT SDL3_FOUND)
  set(SDL_SHARED ON CACHE BOOL "" FORCE)
  set(SDL_STATIC OFF CACHE BOOL "" FORCE)
  set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
  set(SDL_TESTS OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
        SDL3
        URL
            https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.4.4.tar.gz
    )

  FetchContent_MakeAvailable(SDL3)
endif()

if(NOT EnTT_FOUND)
  FetchContent_Declare(
        EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.16.0
        GIT_SHALLOW TRUE
    )

  FetchContent_MakeAvailable(EnTT)
endif()

add_executable(
    idlegolem
    src/main.cpp
    src/engine/sdl_application.cpp
    src/game/fleshgolem_game.cpp
    src/game/fleshgolem_game_gameplay.cpp
    src/game/fleshgolem_game_render.cpp
)

target_include_directories(idlegolem PRIVATE src)

target_link_libraries(idlegolem PRIVATE EnTT::EnTT SDL3::SDL3)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  file(
        CREATE_LINK
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
        SYMBOLIC
    )
endif()
