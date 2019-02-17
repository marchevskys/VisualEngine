#ifndef SHADOWCASCADE_H
#define SHADOWCASCADE_H
#include <array>
#include <glm/glm.hpp>

namespace Visual {
#define SHADOW_MAP_CASCADE_COUNT 4

typedef unsigned int GLuint;
typedef int GLsizei;
class Camera;

class ShadowCascade {
  public:
    ShadowCascade(int size);
    void updateCascades(const Camera &camera);
    void draw();

    struct Cascade {
        float splitDepth;
        glm::mat4 viewProjMatrix;
    };

  private:
    GLuint mCascadedShadowFBO = 0, mCascadedTextureArray = 0;
    GLsizei mShadowMapSize;
    glm::mat4 lightViewMatrix;
    glm::mat4 lightOrthoMatrix;
    std::array<Cascade, SHADOW_MAP_CASCADE_COUNT> cascades;
};
} // namespace Visual
#endif // SHADOWCASCADE_H
