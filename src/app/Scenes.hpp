#pragma once

#include <random>

#include "physics/World.hpp"

namespace app {

constexpr int kSceneCount = 4;
constexpr std::uint32_t kSeed = 42;

const char* sceneName(int index);

// Clears the world, reseeds the RNG and builds scene `index` (0-based).
void loadScene(int index, phys::World& world, std::mt19937& rng);

// Spawns up to `count` random spheres above the floor, rejecting overlaps. Returns how many were
// actually added.
int spawnSpheres(phys::World& world, std::mt19937& rng, int count, double yMin, double yMax);

} // namespace app
