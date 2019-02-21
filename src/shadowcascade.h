#ifndef SHADOWCASCADE_H
#define SHADOWCASCADE_H
#include <array>
#include <functional>

#include <glm/glm.hpp>

namespace Visual {
#define SHADOW_MAP_CASCADE_COUNT 4

typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
class Camera;

class ShadowCascade {
  public:
    ShadowCascade(int size = 1024);
    void prepareCascades(const Camera &camera);

    void drawAll(std::function<void(const glm::mat4 &)> renderFunction);

    GLuint getDepthArrayTextureID();

    struct Cascade {
        float splitDepth;
        glm::mat4 viewProjMatrix;
    };

  private:
    GLuint mCascadedShadowFBO = 0, m_CascadedTextureArray = 0;
    float cascadeSplitLambda = 0.95f;
    GLsizei mShadowMapSize;
    std::array<Cascade, SHADOW_MAP_CASCADE_COUNT> m_cascades;
};
} // namespace Visual
#endif // SHADOWCASCADE_H
