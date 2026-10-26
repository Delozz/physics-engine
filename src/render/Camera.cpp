#include "render/Camera.hpp"

#include <algorithm>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace render {

namespace {

constexpr float kMinDistance = 2.0f;
constexpr float kMaxDistance = 60.0f;
constexpr float kMaxPitch = 85.0f;

} // namespace

void Camera::reset() {
    *this = Camera{};
}

void Camera::orbit(float dYaw, float dPitch) {
    yaw_ += dYaw;
    pitch_ = std::clamp(pitch_ + dPitch, -kMaxPitch, kMaxPitch);
}

void Camera::zoom(float scrollOffset) {
    distance_ = std::clamp(distance_ * std::pow(0.9f, scrollOffset), kMinDistance, kMaxDistance);
}

glm::vec3 Camera::position() const {
    const float yaw = glm::radians(yaw_);
    const float pitch = glm::radians(pitch_);
    const glm::vec3 offset{std::cos(pitch) * std::sin(yaw), std::sin(pitch),
                           std::cos(pitch) * std::cos(yaw)};
    return target_ + distance_ * offset;
}

glm::mat4 Camera::view() const {
    return glm::lookAt(position(), target_, {0.0f, 1.0f, 0.0f});
}

glm::mat4 Camera::projection(float aspect) const {
    return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 200.0f);
}

} // namespace render
