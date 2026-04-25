cmake_minimum_required(VERSION 3.24)

project(idlegolem
    VERSION 0.1.0
    DESCRIPTION "Incremental idle RPG about scavenging and fighting for body parts"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(IDLEGOLEM_USE_SYSTEM_SDL3 "Prefer an installed SDL3 package before fetching it" ON)

if(IDLEGOLEM_USE_SYSTEM_SDL3)
    find_package(SDL3 3.4 CONFIG QUIET)
endif()

if(NOT SDL3_FOUND)
    include(FetchContent)

    set(SDL_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL3
        URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.4.0.tar.gz
    )

    FetchContent_MakeAvailable(SDL3)
endif()

add_executable(idlegolem
    src/main.cpp
)

target_link_libraries(idlegolem
    PRIVATE
        SDL3::SDL3
)
