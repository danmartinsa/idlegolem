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
option(
    IDLEGOLEM_USE_SYSTEM_GLM
    "Prefer an installed GLM package before fetching it"
    ON
)
option(
    IDLEGOLEM_USE_SYSTEM_SDL3_TTF
    "Prefer an installed SDL3_ttf package before fetching it"
    ON
)

if(IDLEGOLEM_USE_SYSTEM_SDL3)
    find_package(SDL3 3.4.4 CONFIG QUIET)
endif()

if(IDLEGOLEM_USE_SYSTEM_ENTT)
    find_package(EnTT CONFIG QUIET)
endif()

if(IDLEGOLEM_USE_SYSTEM_GLM)
    find_package(glm CONFIG QUIET)
endif()

if(IDLEGOLEM_USE_SYSTEM_SDL3_TTF)
    find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
endif()

if(NOT SDL3_FOUND OR NOT EnTT_FOUND OR NOT glm_FOUND OR NOT SDL3_ttf_FOUND)
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

if(NOT glm_FOUND)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(glm)
endif()

if(NOT SDL3_ttf_FOUND)
    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    set(SDLTTF_SAMPLES OFF CACHE BOOL "" FORCE)
    set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.2
        GIT_SHALLOW TRUE
        GIT_SUBMODULES_RECURSE TRUE
    )

    FetchContent_MakeAvailable(SDL3_ttf)
endif()

add_executable(
    idlegolem
    src/engine/application.cpp
    src/game/game.cpp
    src/game/resources/resources.cpp
    src/main.cpp
)

target_include_directories(idlegolem PRIVATE src)

target_link_libraries(
    idlegolem
    PRIVATE EnTT::EnTT SDL3::SDL3 SDL3_ttf::SDL3_ttf glm::glm
)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    file(
        CREATE_LINK
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
        SYMBOLIC
    )
endif()
