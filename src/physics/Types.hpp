#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace phys {

struct SphereDesc {
    glm::dvec3 position;
    double radius;
    double mass;
    glm::dvec3 linearVelocity{0, 0, 0};
    glm::dvec3 angularVelocity{0, 0, 0};
    glm::dquat orientation{1, 0, 0, 0};
    double restitution{0.5};
    double friction{0.5};
};

struct PlaneDesc {
    glm::dvec3 normal; // normalized internally
    double offset;
    double restitution{0.5};
    double friction{0.5};
};

// dot(normal, p) - offset is positive on the open side.
struct Plane {
    glm::dvec3 normal;
    double offset;
    double restitution;
    double friction;
};

// Rebuilt every step. The normal points from body A to body B. For sphere-plane contacts the
// plane is always A, encoded as a = -1 - planeIndex.
struct Contact {
    int64_t a = 0;
    uint32_t b = 0;
    glm::dvec3 normal{0.0};
    double depth = 0.0;
    glm::dvec3 point{0.0};
    glm::dvec3 rA{0.0};
    glm::dvec3 rB{0.0};
    double vn0 = 0.0;
    double jnAcc = 0.0;
    glm::dvec3 jtAcc{0.0};
    double e = 0.0;
    double mu = 0.0;

    bool isPlane() const { return a < 0; }
};

} // namespace phys
