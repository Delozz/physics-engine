#pragma once

#include <span>

#include <glm/glm.hpp>

namespace render {

// Position-only vertex buffer for lines, points and flat quads (grid, floor, contact markers).
class PositionMesh {
public:
    PositionMesh();
    ~PositionMesh();

    PositionMesh(const PositionMesh&) = delete;
    PositionMesh& operator=(const PositionMesh&) = delete;

    void upload(std::span<const glm::vec3> vertices);
    void draw(unsigned int mode) const;

private:
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    int count_ = 0;
};

} // namespace render
