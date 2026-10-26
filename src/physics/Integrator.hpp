#pragma once

#include "physics/RigidSphere.hpp"

namespace phys {

// Phase 1: gravity and accumulated force/torque into velocities, then clear the accumulators.
void integrateVelocity(RigidSphere& s, const glm::dvec3& gravity, double dt);

// Phase 5: semi-implicit Euler for position, exponential map for orientation.
void integratePosition(RigidSphere& s, double dt);

} // namespace phys
