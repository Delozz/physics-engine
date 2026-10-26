#include <cmath>
#include <random>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "app/Scenes.hpp"
#include "physics/World.hpp"

using Catch::Matchers::WithinAbs;

namespace {
constexpr double kDt = 1.0 / 120.0;
}

TEST_CASE("Bounce apex matches e^2 times drop height", "[AC-3]") {
    phys::World world;
    world.addPlane({.normal = {0, 1, 0}, .offset = 0.0, .restitution = 1.0, .friction = 0.0});
    world.addSphere(
        {.position = {0, 1.5, 0}, .radius = 0.5, .mass = 1.0, .restitution = 0.5, .friction = 0.0});

    bool touched = false;
    double apex = 0.0;
    for (int i = 0; i < 600; ++i) {
        world.step(kDt);
        const phys::RigidSphere& s = world.spheres()[0];
        touched = touched || !world.contacts().empty();
        if (!touched) {
            continue;
        }
        apex = std::max(apex, s.position.y);
        if (s.linearVelocity.y < 0.0 && world.contacts().empty()) {
            break; // past the first apex
        }
    }

    const double height = apex - 0.5;
    INFO("rebound apex = " << height << " m");
    REQUIRE(touched);
    CHECK_THAT(height, WithinAbs(0.25, 0.05));
}

TEST_CASE("Inelastic sphere comes to rest on the floor", "[AC-4]") {
    phys::World world;
    world.addPlane({.normal = {0, 1, 0}, .offset = 0.0});
    world.addSphere(
        {.position = {0, 2.5, 0}, .radius = 0.5, .mass = 1.0, .restitution = 0.0, .friction = 0.5});

    for (int i = 0; i < 600; ++i) {
        world.step(kDt);
    }

    const phys::RigidSphere& s = world.spheres()[0];
    INFO("speed = " << glm::length(s.linearVelocity) << ", y = " << s.position.y);
    CHECK(glm::length(s.linearVelocity) < 0.01);
    CHECK_THAT(s.position.y, WithinAbs(0.5, 0.005));
}

TEST_CASE("Sphere pile stays finite and inside the box", "[AC-9]") {
    phys::World world;
    std::mt19937 rng;
    app::loadScene(3, world, rng);
    REQUIRE(world.spheres().size() == 200);

    for (int i = 0; i < 1200; ++i) {
        world.step(kDt);
    }

    double worstPenetration = 0.0;
    for (const phys::RigidSphere& s : world.spheres()) {
        REQUIRE(std::isfinite(s.position.x));
        REQUIRE(std::isfinite(s.position.y));
        REQUIRE(std::isfinite(s.position.z));
        REQUIRE(std::isfinite(glm::length(s.linearVelocity)));
        REQUIRE(std::isfinite(glm::length(s.angularVelocity)));
        REQUIRE(std::isfinite(glm::length(s.orientation)));
        for (const phys::Plane& p : world.planes()) {
            const double d = glm::dot(p.normal, s.position) - p.offset;
            worstPenetration = std::max(worstPenetration, s.radius - d);
        }
    }
    INFO("worst plane penetration = " << worstPenetration << " m");
    CHECK(worstPenetration <= 0.01);
}
