#include "physics/World.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <limits>
#include <stdexcept>

#include "physics/Collision.hpp"
#include "physics/Integrator.hpp"
#include "physics/Solver.hpp"

namespace phys {

namespace {

constexpr double kMaxLinearSpeed = 20.0;   // m/s, keeps sphere-sphere tunneling impossible
constexpr double kMaxAngularSpeed = 100.0; // rad/s

void clampLength(glm::dvec3& v, double maxLength) {
    const double len = glm::length(v);
    if (len > maxLength) {
        v *= maxLength / len;
    }
}

// False for NaN and infinity alike, since both fail the comparison.
bool isFinite(const glm::dvec3& v) {
    return glm::all(glm::lessThan(glm::abs(v), glm::dvec3(std::numeric_limits<double>::max())));
}

} // namespace

uint32_t World::addSphere(const SphereDesc& desc) {
    if (!(desc.radius > 0.0)) {
        throw std::invalid_argument("SphereDesc.radius must be > 0");
    }
    if (!(desc.mass > 0.0)) {
        throw std::invalid_argument("SphereDesc.mass must be > 0");
    }

    RigidSphere s;
    s.id = static_cast<uint32_t>(spheres_.size());
    s.position = desc.position;
    s.linearVelocity = desc.linearVelocity;
    s.angularVelocity = desc.angularVelocity;
    s.orientation = glm::normalize(desc.orientation);
    s.radius = desc.radius;
    s.mass = desc.mass;
    s.invMass = 1.0 / desc.mass;
    s.invInertia = 1.0 / (0.4 * desc.mass * desc.radius * desc.radius);
    s.restitution = desc.restitution;
    s.friction = desc.friction;
    spheres_.push_back(s);
    return s.id;
}

void World::addPlane(const PlaneDesc& desc) {
    const double len = glm::length(desc.normal);
    if (!(len > 0.0)) {
        throw std::invalid_argument("PlaneDesc.normal must be non-zero");
    }
    planes_.push_back({desc.normal / len, desc.offset, desc.restitution, desc.friction});
}

void World::clear() {
    spheres_.clear();
    planes_.clear();
    contacts_.clear();
    time_ = 0.0;
}

void World::setSolverIterations(int iterations) {
    iterations_ = std::clamp(iterations, 1, 100);
}

void World::step(double dt) {
    if (dt <= 0.0) {
        return;
    }

    for (RigidSphere& s : spheres_) {
        if (!s.frozen) {
            integrateVelocity(s, gravity_, dt);
        }
    }

    detectContacts();
    solveVelocities(contacts_, spheres_, iterations_);
    clampSpeeds();

    for (RigidSphere& s : spheres_) {
        if (!s.frozen) {
            integratePosition(s, dt);
        }
    }

    correctPositions(contacts_, spheres_);
    freezeNonFinite();
    time_ += dt;
}

void World::detectContacts() {
    contacts_.clear();

    for (size_t p = 0; p < planes_.size(); ++p) {
        for (const RigidSphere& s : spheres_) {
            if (auto c = collidePlaneSphere(planes_[p], static_cast<int>(p), s)) {
                contacts_.push_back(*c);
            }
        }
    }

    // Brute force, each unordered pair once in ascending id order.
    for (size_t i = 0; i < spheres_.size(); ++i) {
        for (size_t j = i + 1; j < spheres_.size(); ++j) {
            if (auto c = collideSpheres(spheres_[i], spheres_[j])) {
                contacts_.push_back(*c);
            }
        }
    }
}

void World::clampSpeeds() {
    for (RigidSphere& s : spheres_) {
        clampLength(s.linearVelocity, kMaxLinearSpeed);
        clampLength(s.angularVelocity, kMaxAngularSpeed);
    }
}

void World::freezeNonFinite() {
    for (RigidSphere& s : spheres_) {
        if (s.frozen) {
            continue;
        }
        if (isFinite(s.position) && isFinite(s.linearVelocity) && isFinite(s.angularVelocity)) {
            continue;
        }
        assert(!"sphere state went non-finite");
        std::fprintf(stderr, "warning: sphere %u has non-finite state, freezing it\n", s.id);
        s.frozen = true;
        s.linearVelocity = glm::dvec3(0.0);
        s.angularVelocity = glm::dvec3(0.0);
        s.invMass = 0.0;
        s.invInertia = 0.0;
    }
}

} // namespace phys
