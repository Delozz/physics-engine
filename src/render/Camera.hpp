#pragma once

#include <glm/glm.hpp>

namespace render {

// Orbit camera around a target point. Angles in degrees, distance in meters.
class Camera {
public:
    void reset();
    void orbit(float dYaw, float dPitch);
    void zoom(float scrollOffset);

    glm::vec3 position() const;
    glm::mat4 view() const;
    glm::mat4 projection(float aspect) const;

private:
    glm::vec3 target_{0.0f, 1.0f, 0.0f};
    float distance_ = 15.0f;
    float yaw_ = 30.0f;
    float pitch_ = 25.0f;
};

} // namespace render
