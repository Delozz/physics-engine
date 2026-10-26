cmake_minimum_required(VERSION 3.24)
project(physics_engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    set(WARNING_FLAGS /W4)
else()
    set(WARNING_FLAGS -Wall -Wextra -Wpedantic)
endif()

# --- Dependencies ------------------------------------------------------------

include(FetchContent)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)

FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
    GIT_SHALLOW TRUE)

set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm glfw Catch2)

add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

# --- physics: headless, GLM only ---------------------------------------------

add_library(physics STATIC
    src/physics/World.cpp
    src/physics/Collision.cpp
    src/physics/Solver.cpp
    src/physics/Integrator.cpp)
target_include_directories(physics PUBLIC src)
target_link_libraries(physics PUBLIC glm::glm)
target_compile_options(physics PRIVATE ${WARNING_FLAGS})

# --- render -------------------------------------------------------------------

add_library(render STATIC
    src/render/Window.cpp
    src/render/Shader.cpp
    src/render/SphereMesh.cpp
    src/render/PositionMesh.cpp
    src/render/Camera.cpp
    src/render/Renderer.cpp)
target_include_directories(render PUBLIC src)
target_link_libraries(render PUBLIC physics glad glfw)
target_compile_definitions(render PUBLIC GLFW_INCLUDE_NONE)
target_compile_options(render PRIVATE ${WARNING_FLAGS})

# --- engine_app ---------------------------------------------------------------

add_executable(engine_app
    src/app/main.cpp
    src/app/Scenes.cpp)
target_link_libraries(engine_app PRIVATE render physics)
target_compile_definitions(engine_app PRIVATE SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/")
target_compile_options(engine_app PRIVATE ${WARNING_FLAGS})

# --- tests --------------------------------------------------------------------

# Scenes.cpp only depends on physics, so the AC tests can build the same scenes the viewer shows.
add_executable(physics_tests
    tests/test_integration.cpp
    tests/test_collision.cpp
    tests/test_friction.cpp
    tests/test_momentum.cpp
    tests/test_determinism.cpp
    tests/test_benchmark.cpp
    src/app/Scenes.cpp)
target_link_libraries(physics_tests PRIVATE physics Catch2::Catch2WithMain)
target_compile_options(physics_tests PRIVATE ${WARNING_FLAGS})

enable_testing()
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(physics_tests)
