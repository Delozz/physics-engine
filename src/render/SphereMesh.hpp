#pragma once

#include <span>

#include <glm/glm.hpp>

namespace render {

struct SphereInstance {
    glm::mat4 model;
    glm::vec3 color;
};

// Unit UV sphere (32 stacks x 64 slices) drawn instanced, one SphereInstance per body.
class SphereMesh {
public:
    SphereMesh();
    ~SphereMesh();

    SphereMesh(const SphereMesh&) = delete;
    SphereMesh& operator=(const SphereMesh&) = delete;

    void draw(std::span<const SphereInstance> instances);

private:
    unsigned int vao_ = 0;
    unsigned int vertexBuffer_ = 0;
    unsigned int indexBuffer_ = 0;
    unsigned int instanceBuffer_ = 0;
    int indexCount_ = 0;
};

} // namespace render
