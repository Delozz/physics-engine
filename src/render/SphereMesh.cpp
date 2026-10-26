#include "render/SphereMesh.hpp"

#include <cmath>
#include <cstddef>
#include <numbers>
#include <vector>

#include <glad/gl.h>

namespace render {

namespace {

constexpr int kStacks = 32;
constexpr int kSlices = 64;

} // namespace

SphereMesh::SphereMesh() {
    // On a unit sphere the position is also the normal, so one vec3 per vertex is enough.
    std::vector<glm::vec3> vertices;
    for (int i = 0; i <= kStacks; ++i) {
        const float phi = std::numbers::pi_v<float> * static_cast<float>(i) / kStacks;
        for (int j = 0; j <= kSlices; ++j) {
            const float theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(j) / kSlices;
            vertices.emplace_back(std::sin(phi) * std::cos(theta), std::cos(phi),
                                  std::sin(phi) * std::sin(theta));
        }
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i < kStacks; ++i) {
        for (int j = 0; j < kSlices; ++j) {
            const unsigned int a = i * (kSlices + 1) + j;
            const unsigned int b = a + kSlices + 1;
            indices.insert(indices.end(), {a, b, a + 1, a + 1, b, b + 1});
        }
    }
    indexCount_ = static_cast<int>(indices.size());

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vertexBuffer_);
    glGenBuffers(1, &indexBuffer_);
    glGenBuffers(1, &instanceBuffer_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                 GL_STATIC_DRAW);

    // Per-instance model matrix (locations 1-4, one column each) and color (location 5).
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer_);
    for (int col = 0; col < 4; ++col) {
        const GLuint loc = 1 + col;
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(
            loc, 4, GL_FLOAT, GL_FALSE, sizeof(SphereInstance),
            reinterpret_cast<void*>(offsetof(SphereInstance, model) + col * sizeof(glm::vec4)));
        glVertexAttribDivisor(loc, 1);
    }
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(SphereInstance),
                          reinterpret_cast<void*>(offsetof(SphereInstance, color)));
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
}

SphereMesh::~SphereMesh() {
    glDeleteBuffers(1, &instanceBuffer_);
    glDeleteBuffers(1, &indexBuffer_);
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteVertexArrays(1, &vao_);
}

void SphereMesh::draw(std::span<const SphereInstance> instances) {
    if (instances.empty()) {
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer_);
    glBufferData(GL_ARRAY_BUFFER, instances.size_bytes(), instances.data(), GL_STREAM_DRAW);

    glBindVertexArray(vao_);
    glDrawElementsInstanced(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr,
                            static_cast<GLsizei>(instances.size()));
    glBindVertexArray(0);
}

} // namespace render
