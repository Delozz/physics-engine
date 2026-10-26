#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "physics/RigidSphere.hpp"
#include "physics/Types.hpp"

namespace phys {

class World {
public:
    uint32_t addSphere(const SphereDesc& desc); // throws std::invalid_argument
    void addPlane(const PlaneDesc& desc);       // throws std::invalid_argument
    void clear();
    void step(double dt);

    void setGravity(glm::dvec3 gravity) { gravity_ = gravity; }
    void setSolverIterations(int iterations);

    std::span<const RigidSphere> spheres() const { return spheres_; }
    std::span<const Plane> planes() const { return planes_; }
    std::span<const Contact> contacts() const { return contacts_; }
    double time() const { return time_; }
    glm::dvec3 gravity() const { return gravity_; }

private:
    void detectContacts();
    void clampSpeeds();
    void freezeNonFinite();

    std::vector<RigidSphere> spheres_; // index == id
    std::vector<Plane> planes_;
    std::vector<Contact> contacts_;
    glm::dvec3 gravity_{0.0, -9.81, 0.0};
    int iterations_ = 10;
    double time_ = 0.0;
};

} // namespace phys
