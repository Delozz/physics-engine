#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace phys {

struct RigidSphere {
    uint32_t id = 0;
    glm::dvec3 position{0.0};
    glm::dvec3 linearVelocity{0.0};
    glm::dquat orientation{1.0, 0.0, 0.0, 0.0};
    glm::dvec3 angularVelocity{0.0}; // world space
    glm::dvec3 force{0.0};
    glm::dvec3 torque{0.0};
    double radius = 1.0;
    double mass = 1.0;
    double invMass = 1.0;
    double invInertia = 2.5; // 1 / (0.4 m r^2)
    double restitution = 0.5;
    double friction = 0.5;
    bool frozen = false; // set when the state goes non-finite
};

} // namespace phys
