#include "render/PositionMesh.hpp"

#include <glad/gl.h>

namespace render {

PositionMesh::PositionMesh() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glBindVertexArray(0);
}

PositionMesh::~PositionMesh() {
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}

void PositionMesh::upload(std::span<const glm::vec3> vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_DYNAMIC_DRAW);
    count_ = static_cast<int>(vertices.size());
}

void PositionMesh::draw(unsigned int mode) const {
    if (count_ == 0) {
        return;
    }
    glBindVertexArray(vao_);
    glDrawArrays(mode, 0, count_);
    glBindVertexArray(0);
}

} // namespace render
