#include "app/Scenes.hpp"

#include <array>
#include <numbers>

namespace app {

namespace {

const glm::dvec3 kGravity{0.0, -9.81, 0.0};

void dropAndBounce(phys::World& world) {
    world.setGravity(kGravity);
    world.addPlane({.normal = {0, 1, 0}, .offset = 0.0, .restitution = 1.0, .friction = 0.5});

    const std::array<double, 5> restitution{0.0, 0.25, 0.5, 0.75, 1.0};
    for (int i = 0; i < 5; ++i) {
        world.addSphere({.position = {-4.0 + 2.0 * i, 5.0, 0.0},
                         .radius = 0.5,
                         .mass = 1.0,
                         .restitution = restitution[i],
                         .friction = 0.5});
    }
}

void slideToRoll(phys::World& world) {
    world.setGravity(kGravity);
    world.addPlane({.normal = {0, 1, 0}, .offset = 0.0, .restitution = 0.0, .friction = 0.5});
    world.addSphere({.position = {-5.0, 0.5, 0.0},
                     .radius = 0.5,
                     .mass = 1.0,
                     .linearVelocity = {5.0, 0.0, 0.0},
                     .restitution = 0.0,
                     .friction = 0.5});
}

void collisions(phys::World& world) {
    world.setGravity({0.0, 0.0, 0.0});

    // Pair A: head-on, elastic, frictionless.
    world.addSphere({.position = {-3, 1, 0},
                     .radius = 0.5,
                     .mass = 1.0,
                     .linearVelocity = {2, 0, 0},
                     .restitution = 1.0,
                     .friction = 0.0});
    world.addSphere({.position = {3, 1, 0},
                     .radius = 0.5,
                     .mass = 1.0,
                     .linearVelocity = {-2, 0, 0},
                     .restitution = 1.0,
                     .friction = 0.0});

    // Pair B: offset in z so the hit is glancing, with friction to spin them up.
    world.addSphere({.position = {-3, 4, 0},
                     .radius = 0.5,
                     .mass = 1.0,
                     .linearVelocity = {2, 0, 0},
                     .restitution = 0.5,
                     .friction = 0.4});
    world.addSphere({.position = {3, 4, 0.6},
                     .radius = 0.5,
                     .mass = 1.0,
                     .linearVelocity = {-2, 0, 0},
                     .restitution = 0.5,
                     .friction = 0.4});
}

void spherePile(phys::World& world, std::mt19937& rng) {
    world.setGravity(kGravity);

    const double e = 0.3;
    const double mu = 0.5;
    world.addPlane({.normal = {0, 1, 0}, .offset = 0.0, .restitution = e, .friction = mu});
    world.addPlane({.normal = {1, 0, 0}, .offset = -5.0, .restitution = e, .friction = mu});
    world.addPlane({.normal = {-1, 0, 0}, .offset = -5.0, .restitution = e, .friction = mu});
    world.addPlane({.normal = {0, 0, 1}, .offset = -5.0, .restitution = e, .friction = mu});
    world.addPlane({.normal = {0, 0, -1}, .offset = -5.0, .restitution = e, .friction = mu});

    spawnSpheres(world, rng, 200, 1.0, 12.0);
}

} // namespace

const char* sceneName(int index) {
    static constexpr std::array<const char*, kSceneCount> names{"Drop and Bounce", "Slide to Roll",
                                                                "Collisions", "Sphere Pile"};
    return names[static_cast<size_t>(index)];
}

void loadScene(int index, phys::World& world, std::mt19937& rng) {
    world.clear();
    rng.seed(kSeed);

    switch (index) {
    case 0:
        dropAndBounce(world);
        break;
    case 1:
        slideToRoll(world);
        break;
    case 2:
        collisions(world);
        break;
    case 3:
        spherePile(world, rng);
        break;
    default:
        break;
    }
}

int spawnSpheres(phys::World& world, std::mt19937& rng, int count, double yMin, double yMax) {
    std::uniform_real_distribution<double> radiusDist(0.25, 0.5);
    std::uniform_real_distribution<double> xzDist(-4.0, 4.0);
    std::uniform_real_distribution<double> yDist(yMin, yMax);

    int added = 0;
    for (int n = 0; n < count; ++n) {
        const double r = radiusDist(rng);

        for (int attempt = 0; attempt < 100; ++attempt) {
            const glm::dvec3 p{xzDist(rng), yDist(rng), xzDist(rng)};

            bool overlaps = false;
            for (const phys::RigidSphere& s : world.spheres()) {
                if (glm::length(p - s.position) < r + s.radius + 0.05) {
                    overlaps = true;
                    break;
                }
            }
            if (overlaps) {
                continue;
            }

            const double mass = 1000.0 * (4.0 / 3.0) * std::numbers::pi * r * r * r;
            world.addSphere(
                {.position = p, .radius = r, .mass = mass, .restitution = 0.3, .friction = 0.5});
            ++added;
            break;
        }
    }
    return added;
}

} // namespace app
