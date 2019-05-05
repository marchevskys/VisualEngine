#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>

namespace Visual {
class Camera {
  public:
    Camera(glm::dvec3 pos, glm::dvec3 aim);
    virtual ~Camera() {}

    void set(glm::dvec3 pos, glm::dvec3 aim, glm::dvec3 up = {0.0, 0.0, 1.0});

    template <class Precision = float>
    glm::mat<4, 4, Precision, glm::highp> getView() const { return m_view; }
    const glm::mat4 &getProjection() const { return m_projection; };
    //virtual float getDistance() const { glm::length(m_aim - m_pos); }
    glm::vec3 getPos() { return m_pos; }

    void setFOV(float fov);
    void setAR(float ar);

    float getNear() const { return m_near; }
    float getFar() const { return m_far; }
    float getAR() const { return m_aspectRatio; }

  protected:
    float m_near = .1f, m_far = 2000.f, m_fov = 1.f, m_aspectRatio = 1.f;

    glm::vec3 m_pos;
    glm::mat4 m_projection;
    glm::mat4 m_view, m_inverse;

    void updateProjection();
};

// trackable camera with Z up direction
class CameraRotateUpDirect : public Camera {

  public:
    CameraRotateUpDirect(glm::dvec3 pos, glm::dvec3 aim) : Camera(pos, aim) {}
    float getDistance() const { return m_dist; }
    void update(glm::vec3 pos, float dx, float dy, float distance);

  private:
    float m_angleX = 0.f, m_angleY = 0.f, m_dist = 6.0f;
};

class CameraRotateOmniDirect : public Camera {

  public:
    CameraRotateOmniDirect(glm::dvec3 pos, glm::dvec3 aim) : Camera(pos, aim) {
        //m_dir = glm::normalize(aim - pos);
        m_up = {0.f, 0.f, 1.f};
        m_right = {0.f, 1.f, 0.f};
    }
    float getDistance() const { return m_dist; }
    void update(glm::vec3 pos, float dx, float dy, float distance);

  private:
    float m_dist = 6.0f;
    glm::vec3 m_right, m_up;
};

} // namespace Visual
#endif // CAMERA_H
