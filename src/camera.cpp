#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace Visual {
void Camera::updateProjection() { m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far); }

Camera::Camera(vec3d pos, vec3d aim) : m_pos(pos), m_aim(aim), m_up({0.0, 0.0, 1.0}) {
    updateProjection();
}

void Camera::setFOV(float fov) {
    m_fov = fov;
    updateProjection();
}

void Camera::setAspectRatio(float ar) {
    m_aspectRatio = ar;
    updateProjection();
}

mat4d Camera::getView() { return glm::lookAt(m_pos, m_aim, m_up); }

void Camera::move(vec3d offset) {
    m_pos += offset;
    m_aim += offset;
}

void Camera::rotate(vec3d angle) {
    //vec3d m_pos, m_aim, m_up;
    //glm::rotate()
}

} // namespace Visual
