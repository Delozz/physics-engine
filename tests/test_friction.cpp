#include <span>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "app/Scenes.hpp"
#include "physics/Collision.hpp"
#include "physics/Solver.hpp"
#include "physics/World.hpp"

using Catch::Matchers::WithinRel;

TEST_CASE("Sliding sphere settles into rolling at 5/7 of its speed", "[AC-5]") {
    phys::World world;
    std::mt19937 rng;
    app::loadScene(1, world, rng);

    for (int i = 0; i < 120; ++i) {
        world.step(1.0 / 120.0);
    }

    const phys::RigidSphere& s = world.spheres()[0];
    const double vx = s.linearVelocity.x;
    INFO("vx = " << vx << ", omega.z = " << s.angularVelocity.z);
    CHECK_THAT(vx, WithinRel(5.0 * 5.0 / 7.0, 0.03));
    CHECK_THAT(s.angularVelocity.z, WithinRel(-vx / s.radius, 0.03));
}

TEST_CASE("Friction impulse never leaves the Coulomb cone", "[AC-8]") {
    phys::RigidSphere sphere;
    sphere.id = 0;
    sphere.position = {0.0, 0.49, 0.0};
    sphere.linearVelocity = {10.0, -1.0, 3.0};
    sphere.radius = 0.5;
    sphere.mass = 1.0;
    sphere.invMass = 1.0;
    sphere.invInertia = 1.0 / (0.4 * 1.0 * 0.25);
    sphere.friction = 0.1;

    const phys::Plane floor{{0, 1, 0}, 0.0, 0.0, 0.1};
    auto contact = phys::collidePlaneSphere(floor, 0, sphere);
    REQUIRE(contact);

    std::vector<phys::Contact> contacts{*contact};
    std::vector<phys::RigidSphere> spheres{sphere};
    phys::solveVelocities(contacts, spheres, 10);

    const phys::Contact& c = contacts[0];
    // The unclamped impulse would stop the 10.4 m/s slip outright: 2m/7 * 10.4 is about 3 N s.
    REQUIRE(c.mu * c.jnAcc < 1.0);
    INFO("|jt| = " << glm::length(c.jtAcc) << ", mu * jn = " << c.mu * c.jnAcc);
    CHECK(glm::length(c.jtAcc) <= c.mu * c.jnAcc + 1e-12);
}
