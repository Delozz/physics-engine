#include "physics/Integrator.hpp"

namespace phys {

void integrateVelocity(RigidSphere& s, const glm::dvec3& gravity, double dt) {
    s.linearVelocity += (gravity + s.force * s.invMass) * dt;
    s.angularVelocity += s.invInertia * s.torque * dt;
    s.force = glm::dvec3(0.0);
    s.torque = glm::dvec3(0.0);
}

void integratePosition(RigidSphere& s, double dt) {
    s.position += s.linearVelocity * dt;

    // q += 0.5 * dt * w * q drifts as |w| dt grows; rotating by the exact angle does not.
    const double w = glm::length(s.angularVelocity);
    if (w >= 1e-12) {
        const glm::dquat dq = glm::angleAxis(w * dt, s.angularVelocity / w);
        s.orientation = dq * s.orientation;
    }
    s.orientation = glm::normalize(s.orientation);
}

} // namespace phys
