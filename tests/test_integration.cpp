#include <cmath>
#include <numbers>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "physics/World.hpp"

using Catch::Matchers::WithinAbs;

namespace {
constexpr double kDt = 1.0 / 120.0;
}

TEST_CASE("Free fall matches 10 - g t^2 / 2 after one second", "[AC-1]") {
    phys::World world;
    world.addSphere({.position = {0, 10, 0}, .radius = 0.5, .mass = 1.0});

    for (int i = 0; i < 120; ++i) {
        world.step(kDt);
    }

    const double expected = 10.0 - 0.5 * 9.81 * 1.0;
    const double y = world.spheres()[0].position.y;
    INFO("y = " << y << ", expected " << expected);
    CHECK_THAT(y, WithinAbs(expected, 0.05));
}

TEST_CASE("Exponential map keeps orientation unit length and exact", "[AC-2]") {
    phys::World world;
    world.setGravity({0, 0, 0});
    world.addSphere(
        {.position = {0, 0, 0}, .radius = 0.5, .mass = 1.0, .angularVelocity = {0, 0, 10}});

    constexpr int steps = 10'000;
    for (int i = 0; i < steps; ++i) {
        world.step(kDt);
    }

    const glm::dquat q = world.spheres()[0].orientation;
    CHECK(std::abs(glm::length(q) - 1.0) < 1e-9);

    // Pure z rotation: q = (cos(a/2), 0, 0, sin(a/2)).
    const double angle = 2.0 * std::atan2(q.z, q.w);
    const double expected = 10.0 * steps * kDt;
    const double error = std::remainder(angle - expected, 2.0 * std::numbers::pi);
    INFO("angle error = " << error << " rad");
    CHECK(std::abs(error) < 1e-6);
}
