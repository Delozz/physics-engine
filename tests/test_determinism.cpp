#include <cstring>
#include <random>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "app/Scenes.hpp"
#include "physics/World.hpp"

namespace {

struct Snapshot {
    std::vector<glm::dvec3> positions;
    std::vector<glm::dquat> orientations;
};

Snapshot runPile() {
    phys::World world;
    std::mt19937 rng;
    app::loadScene(3, world, rng);
    for (int i = 0; i < 1200; ++i) {
        world.step(1.0 / 120.0);
    }

    Snapshot snap;
    for (const phys::RigidSphere& s : world.spheres()) {
        snap.positions.push_back(s.position);
        snap.orientations.push_back(s.orientation);
    }
    return snap;
}

} // namespace

TEST_CASE("Same scene and seed gives bitwise identical state", "[AC-10]") {
    const Snapshot a = runPile();
    const Snapshot b = runPile();

    REQUIRE(a.positions.size() == b.positions.size());
    CHECK(std::memcmp(a.positions.data(), b.positions.data(),
                      a.positions.size() * sizeof(glm::dvec3)) == 0);
    CHECK(std::memcmp(a.orientations.data(), b.orientations.data(),
                      a.orientations.size() * sizeof(glm::dquat)) == 0);
}
