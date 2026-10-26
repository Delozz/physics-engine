#pragma once

#include <optional>

#include "physics/RigidSphere.hpp"
#include "physics/Types.hpp"

namespace phys {

// Detection only. Fills normal, depth, point, offsets, combined material and the pre-solve
// normal speed vn0. Impulse accumulators start at zero.
std::optional<Contact> collidePlaneSphere(const Plane& plane, int planeIndex,
                                          const RigidSphere& sphere);

// Expects a.id < b.id so that A is always the lower id.
std::optional<Contact> collideSpheres(const RigidSphere& a, const RigidSphere& b);

} // namespace phys
