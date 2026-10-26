#include "physics/Collision.hpp"

#include <algorithm>
#include <cmath>

namespace phys {

std::optional<Contact> collidePlaneSphere(const Plane& plane, int planeIndex,
                                          const RigidSphere& sphere) {
    // Also true when the center is behind the plane, so a fast sphere gets pushed back out.
    const double d = glm::dot(plane.normal, sphere.position) - plane.offset;
    if (d >= sphere.radius) {
        return std::nullopt;
    }

    Contact c;
    c.a = -1 - static_cast<int64_t>(planeIndex);
    c.b = sphere.id;
    c.normal = plane.normal;
    c.depth = sphere.radius - d;
    // Midpoint between the deepest point of the sphere and its projection on the plane.
    c.point = sphere.position - plane.normal * (0.5 * (sphere.radius + d));
    c.rA = glm::dvec3(0.0);
    c.rB = c.point - sphere.position;
    c.e = std::min(plane.restitution, sphere.restitution);
    c.mu = std::sqrt(plane.friction * sphere.friction);

    const glm::dvec3 vB = sphere.linearVelocity + glm::cross(sphere.angularVelocity, c.rB);
    c.vn0 = glm::dot(vB, c.normal);
    return c;
}

std::optional<Contact> collideSpheres(const RigidSphere& a, const RigidSphere& b) {
    const glm::dvec3 delta = b.position - a.position;
    const double radiusSum = a.radius + b.radius;
    const double dist2 = glm::dot(delta, delta);
    if (dist2 >= radiusSum * radiusSum) {
        return std::nullopt;
    }

    const double dist = std::sqrt(dist2);

    Contact c;
    c.a = a.id;
    c.b = b.id;
    c.normal = dist < 1e-9 ? glm::dvec3(0.0, 1.0, 0.0) : delta / dist;
    c.depth = radiusSum - dist;
    c.point = a.position + c.normal * (a.radius - 0.5 * c.depth);
    c.rA = c.point - a.position;
    c.rB = c.point - b.position;
    c.e = std::min(a.restitution, b.restitution);
    c.mu = std::sqrt(a.friction * b.friction);

    const glm::dvec3 vA = a.linearVelocity + glm::cross(a.angularVelocity, c.rA);
    const glm::dvec3 vB = b.linearVelocity + glm::cross(b.angularVelocity, c.rB);
    c.vn0 = glm::dot(vB - vA, c.normal);
    return c;
}

} // namespace phys
