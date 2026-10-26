#pragma once

#include <string>
#include <vector>

#include "physics/World.hpp"
#include "render/Camera.hpp"
#include "render/PositionMesh.hpp"
#include "render/Shader.hpp"
#include "render/SphereMesh.hpp"

namespace render {

// Draws a read-only view of the world: instanced spheres, a grid on every +Y plane and
// optional contact markers.
class Renderer {
public:
    explicit Renderer(const std::string& shaderDir);

    void draw(const phys::World& world, const Camera& camera, int width, int height,
              bool showContacts);

private:
    Shader sphereShader_;
    Shader flatShader_;
    SphereMesh sphereMesh_;
    PositionMesh gridLines_;
    PositionMesh floorQuad_;
    PositionMesh contactPoints_;
    std::vector<SphereInstance> instances_;
    std::vector<glm::vec3> contactScratch_;
};

} // namespace render
