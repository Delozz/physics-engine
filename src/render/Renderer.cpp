#include "render/Renderer.hpp"

#include <cmath>

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace render {

namespace {

constexpr float kGridHalfExtent = 20.0f;
const glm::vec3 kBackground{0.08f, 0.08f, 0.10f};
const glm::vec3 kFloorColor{0.20f, 0.20f, 0.22f};
const glm::vec3 kGridColor{0.42f, 0.42f, 0.45f};
const glm::vec3 kContactColor{1.0f, 0.25f, 0.2f};
const glm::vec3 kLightDir = glm::normalize(glm::vec3{0.4f, 1.0f, 0.3f});

// Spread hues with the golden ratio so neighbouring ids never look alike.
glm::vec3 colorForId(uint32_t id) {
    const float h = std::fmod(0.13f + static_cast<float>(id) * 0.618034f, 1.0f) * 6.0f;
    const float s = 0.6f;
    const float v = 0.9f;
    const float c = v * s;
    const float x = c * (1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f));
    glm::vec3 rgb;
    switch (static_cast<int>(h)) {
    case 0:
        rgb = {c, x, 0};
        break;
    case 1:
        rgb = {x, c, 0};
        break;
    case 2:
        rgb = {0, c, x};
        break;
    case 3:
        rgb = {0, x, c};
        break;
    case 4:
        rgb = {x, 0, c};
        break;
    default:
        rgb = {c, 0, x};
        break;
    }
    return rgb + glm::vec3(v - c);
}

std::vector<glm::vec3> gridVertices() {
    std::vector<glm::vec3> v;
    const float y = 0.002f; // just above the floor quad to avoid z-fighting
    for (int i = -20; i <= 20; ++i) {
        const float t = static_cast<float>(i);
        v.insert(v.end(), {{t, y, -kGridHalfExtent}, {t, y, kGridHalfExtent}});
        v.insert(v.end(), {{-kGridHalfExtent, y, t}, {kGridHalfExtent, y, t}});
    }
    return v;
}

std::vector<glm::vec3> floorVertices() {
    const float e = kGridHalfExtent;
    return {{-e, 0, -e}, {e, 0, -e}, {e, 0, e}, {-e, 0, -e}, {e, 0, e}, {-e, 0, e}};
}

} // namespace

Renderer::Renderer(const std::string& shaderDir)
    : sphereShader_(shaderDir + "sphere.vert", shaderDir + "sphere.frag"),
      flatShader_(shaderDir + "grid.vert", shaderDir + "grid.frag") {
    gridLines_.upload(gridVertices());
    floorQuad_.upload(floorVertices());
}

void Renderer::draw(const phys::World& world, const Camera& camera, int width, int height,
                    bool showContacts) {
    glViewport(0, 0, width, height);
    glClearColor(kBackground.r, kBackground.g, kBackground.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    const glm::mat4 viewProj = camera.projection(aspect) * camera.view();

    flatShader_.use();
    for (const phys::Plane& plane : world.planes()) {
        if (plane.normal != glm::dvec3(0.0, 1.0, 0.0)) {
            continue;
        }
        const glm::mat4 model =
            glm::translate(glm::mat4(1.0f), {0.0f, static_cast<float>(plane.offset), 0.0f});
        flatShader_.set("uMVP", viewProj * model);
        flatShader_.set("uColor", kFloorColor);
        floorQuad_.draw(GL_TRIANGLES);
        flatShader_.set("uColor", kGridColor);
        gridLines_.draw(GL_LINES);
    }

    instances_.clear();
    for (const phys::RigidSphere& s : world.spheres()) {
        const glm::mat4 model =
            glm::translate(glm::mat4(1.0f), glm::vec3(s.position)) *
            glm::mat4_cast(glm::quat(s.orientation)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(static_cast<float>(s.radius)));
        instances_.push_back({model, colorForId(s.id)});
    }
    sphereShader_.use();
    sphereShader_.set("uViewProj", viewProj);
    sphereShader_.set("uCameraPos", camera.position());
    sphereShader_.set("uLightDir", kLightDir);
    sphereMesh_.draw(instances_);

    if (showContacts && !world.contacts().empty()) {
        contactScratch_.clear();
        for (const phys::Contact& c : world.contacts()) {
            contactScratch_.emplace_back(c.point);
        }
        contactPoints_.upload(contactScratch_);
        glDisable(GL_DEPTH_TEST);
        glPointSize(6.0f);
        flatShader_.use();
        flatShader_.set("uMVP", viewProj);
        flatShader_.set("uColor", kContactColor);
        contactPoints_.draw(GL_POINTS);
        glEnable(GL_DEPTH_TEST);
    }
}

} // namespace render
