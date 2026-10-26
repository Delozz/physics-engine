#include <chrono>
#include <cstdio>
#include <random>

#include <catch2/catch_test_macros.hpp>

#include "app/Scenes.hpp"
#include "physics/World.hpp"

// Hidden from the default run. Use: physics_tests "[.benchmark]"

namespace {

double meanStepMs(phys::World& world, int warmup, int samples) {
    for (int i = 0; i < warmup; ++i) {
        world.step(1.0 / 120.0);
    }
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < samples; ++i) {
        world.step(1.0 / 120.0);
    }
    const std::chrono::duration<double, std::milli> elapsed =
        std::chrono::steady_clock::now() - start;
    return elapsed.count() / samples;
}

} // namespace

TEST_CASE("World::step cost with 200 and 500 spheres", "[.benchmark][AC-11]") {
    phys::World world;
    std::mt19937 rng;
    app::loadScene(3, world, rng);
    const double ms200 = meanStepMs(world, 1200, 1000);
    std::printf("200 spheres: %.3f ms/step\n", ms200);
    CHECK(ms200 <= 2.0);

    app::spawnSpheres(world, rng, 300, 12.0, 30.0);
    std::printf("%zu spheres: ", world.spheres().size());
    const double ms500 = meanStepMs(world, 1200, 1000);
    std::printf("%.3f ms/step\n", ms500);
    CHECK(ms500 <= 8.0);
}
