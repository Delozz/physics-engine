#pragma once

#include <span>

#include "physics/RigidSphere.hpp"
#include "physics/Types.hpp"

namespace phys {

// Sequential impulses: normal impulse with restitution, then Coulomb friction clamped to the
// accumulated normal impulse. Spheres are indexed by id.
void solveVelocities(std::span<Contact> contacts, std::span<RigidSphere> spheres, int iterations);

// Pushes bodies apart along the normal by a fraction of the depth beyond the slop.
void correctPositions(std::span<const Contact> contacts, std::span<RigidSphere> spheres);

} // namespace phys
