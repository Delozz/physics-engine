#include "physics/Solver.hpp"

#include <algorithm>

namespace phys {

namespace {

constexpr double kRestitutionThreshold = 0.5; // m/s
constexpr double kSlop = 0.001;               // m
constexpr double kCorrectionFraction = 0.8;

// A plane is a static body: null pointer, zero velocity, zero inverse mass.
RigidSphere* bodyA(const Contact& c, std::span<RigidSphere> spheres) {
    return c.isPlane() ? nullptr : &spheres[static_cast<size_t>(c.a)];
}

double invMassOf(const RigidSphere* s) {
    return s ? s->invMass : 0.0;
}
double invInertiaOf(const RigidSphere* s) {
    return s ? s->invInertia : 0.0;
}

glm::dvec3 velocityAt(const RigidSphere* s, const glm::dvec3& r) {
    return s ? s->linearVelocity + glm::cross(s->angularVelocity, r) : glm::dvec3(0.0);
}

void applyImpulse(RigidSphere* s, const glm::dvec3& r, const glm::dvec3& p) {
    if (!s) {
        return;
    }
    s->linearVelocity += p * s->invMass;
    s->angularVelocity += s->invInertia * glm::cross(r, p);
}

void solveContact(Contact& c, RigidSphere* a, RigidSphere* b) {
    const glm::dvec3& n = c.normal;
    const double invMassSum = invMassOf(a) + b->invMass;
    if (invMassSum == 0.0) {
        return;
    }
    const double mN = 1.0 / invMassSum;
    // Offsets are parallel to n, so for spheres rotation only enters the tangent mass.
    const double mT = 1.0 / (invMassSum + glm::dot(c.rA, c.rA) * invInertiaOf(a) +
                             glm::dot(c.rB, c.rB) * b->invInertia);
    const double vTarget = -c.vn0 > kRestitutionThreshold ? -c.e * c.vn0 : 0.0;

    // Normal
    glm::dvec3 vrel = velocityAt(b, c.rB) - velocityAt(a, c.rA);
    const double vn = glm::dot(vrel, n);
    const double jnNew = std::max(c.jnAcc + mN * (vTarget - vn), 0.0);
    const glm::dvec3 pn = (jnNew - c.jnAcc) * n;
    c.jnAcc = jnNew;
    applyImpulse(a, c.rA, -pn);
    applyImpulse(b, c.rB, pn);

    // Friction
    vrel = velocityAt(b, c.rB) - velocityAt(a, c.rA);
    const glm::dvec3 vt = vrel - glm::dot(vrel, n) * n;
    glm::dvec3 jtNew = c.jtAcc - mT * vt;
    const double maxJt = c.mu * c.jnAcc;
    const double jtLen = glm::length(jtNew);
    if (jtLen > maxJt) {
        jtNew *= maxJt / jtLen;
    }
    const glm::dvec3 pt = jtNew - c.jtAcc;
    c.jtAcc = jtNew;
    applyImpulse(a, c.rA, -pt);
    applyImpulse(b, c.rB, pt);
}

} // namespace

void solveVelocities(std::span<Contact> contacts, std::span<RigidSphere> spheres, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        for (Contact& c : contacts) {
            solveContact(c, bodyA(c, spheres), &spheres[c.b]);
        }
    }
}

void correctPositions(std::span<const Contact> contacts, std::span<RigidSphere> spheres) {
    for (const Contact& c : contacts) {
        RigidSphere* a = bodyA(c, spheres);
        RigidSphere* b = &spheres[c.b];
        const double invMassSum = invMassOf(a) + b->invMass;
        if (invMassSum == 0.0) {
            continue;
        }
        const double corr = std::max(c.depth - kSlop, 0.0) * kCorrectionFraction / invMassSum;
        if (a) {
            a->position -= corr * a->invMass * c.normal;
        }
        b->position += corr * b->invMass * c.normal;
    }
}

} // namespace phys
