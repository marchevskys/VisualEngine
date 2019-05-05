#include "camera.h"
#include "logger.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace Visual {
void Camera::updateProjection() { m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far); }

Camera::Camera(glm::dvec3 pos, glm::dvec3 aim) {
    updateProjection();
    set(pos, aim, {0.0, 0.0, 1.0});
}

void Camera::set(glm::dvec3 pos, glm::dvec3 aim, glm::dvec3 up) {
    m_view = glm::lookAt(pos, aim, up);
    m_pos = pos;
}

void Camera::setFOV(float fov) {
    m_fov = fov;
    updateProjection();
}

void Camera::setAR(float ar) {
    m_aspectRatio = ar;
    updateProjection();
}

void CameraRotateUpDirect::update(glm::vec3 pos, float dx, float dy, float distance) {
    m_angleX += dx;
    m_angleY += dy;
    constexpr float pi2 = M_PI_2;
    m_angleY = glm::clamp(m_angleY, -pi2 + 0.0001f, pi2 - 0.0001f);
    glm::vec3 dir(1, 0, 0);
    dir = glm::rotate(dir, m_angleY, glm::vec3(0, 1, 0));
    dir = glm::rotate(dir, m_angleX, glm::vec3(0, 0, 1));
    m_dist = distance;
    set(pos - dir * m_dist, pos, glm::vec3(0, 0, 1));
}

void CameraRotateOmniDirect::update(glm::vec3 pos, float dx, float dy, float distance) {

    m_right = glm::rotate(m_right, dx, m_up);
    m_up = glm::rotate(m_up, -dy, m_right);
    glm::vec3 dir = glm::cross(m_right, m_up);
    m_dist = distance;
    set(pos + dir * m_dist, pos, m_up);
}

} // namespace Visual
