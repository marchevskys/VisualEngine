#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>

namespace Visual {
class Camera {
  public:
    Camera(glm::dvec3 pos, glm::dvec3 aim);
    void set(glm::dvec3 pos, glm::dvec3 aim, glm::dvec3 up = {0.0, 0.0, 1.0});

    template <class Precision = float>
    glm::mat<4, 4, Precision, glm::highp> getView() const { return m_view; }

    template <class Precision = float>
    glm::vec<3, Precision, glm::highp> getPos() const { return m_pos; }

    template <class Precision = float>
    glm::vec<3, Precision, glm::highp> getDir() const { return m_view[2]; };

    void move(glm::dvec3 offset);
    void rotate(glm::dvec3 angle);

    void setFOV(float fov);
    void setFOV(double fov) { setFOV(static_cast<float>(fov)); };
    void setAR(float ar);

    float getNear() const { return m_near; }
    float getFar() const { return m_far; }
    float getAR() const { return m_aspectRatio; }

    const glm::mat4 &getProjection() const { return m_projection; };

  private:
    float m_near = 0.1f, m_far = 2000.f, m_fov = 1.f, m_aspectRatio = 1.f;

    glm::dvec3 m_pos, m_aim, m_up;
    glm::mat4 m_projection;
    glm::dmat4 m_view;

    void updateView();
    void updateProjection();
};
} // namespace Visual
#endif // CAMERA_H
