#include "camera.h"
#include "logger.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace Visual {
void Camera::updateProjection() { m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far); }

Camera::Camera(glm::dvec3 pos, glm::dvec3 aim) : m_pos(pos), m_aim(aim), m_up({0.0, 0.0, 1.0}) {
    updateProjection();
    updateView();
}

void Camera::set(glm::dvec3 pos, glm::dvec3 aim, glm::dvec3 up) {
    m_pos = pos, m_aim = aim, m_up = up;
    updateView();
}

void Camera::setFOV(float fov) {
    m_fov = fov;
    updateProjection();
}

void Camera::setAR(float ar) {
    m_aspectRatio = ar;
    updateProjection();
}

void Camera::move(glm::vec3 offset) {
    m_pos += offset;
    m_aim += offset;
    updateView();
}

void Camera::updateView() {
    m_view = glm::lookAt(m_pos, m_aim, m_up);
}

void CameraTrackRotate::update(glm::vec3 pos, float x, float y, float distance) {
    m_angleX += x;
    m_angleY += y;
    constexpr float pi2 = M_PI_2;
    m_angleY = glm::clamp(m_angleY, -pi2 + 0.0001f, pi2 - 0.0001f);
    glm::vec3 dir(1, 0, 0);
    dir = glm::rotate(dir, m_angleY, glm::vec3(0, 1, 0));
    dir = glm::rotate(dir, m_angleX, glm::vec3(0, 0, 1));

    set(pos - dir * m_dist, pos, glm::vec3(0, 0, 1));
}

} // namespace Visual
