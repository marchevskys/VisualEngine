#ifndef SHADOWCASCADE_H
#define SHADOWCASCADE_H
#include <array>
#include <functional>

#include <glm/glm.hpp>

namespace Visual {
#define SHADOW_MAP_CASCADE_COUNT 6

typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
class Camera;

class ShadowCascade {
  public:
    ShadowCascade(int size = 2048);
    void prepareCascades(const Camera &camera, const glm::vec3 &lightDir);
    void drawAll(std::function<void(const glm::mat4 &)> renderFunction);
    GLuint getDepthArrayTextureID();

    const float *getSplitDepthes() { return splitDepth; }

    float splitDepth[SHADOW_MAP_CASCADE_COUNT];
    glm::mat4 viewProjMatrix[SHADOW_MAP_CASCADE_COUNT];

  private:
    GLuint mCascadedShadowFBO = 0, m_CascadedTextureArray = 0;
    const float cascadeSplitLambda = 0.95f;
    GLsizei mShadowMapSize;
};
} // namespace Visual
#endif // SHADOWCASCADE_H
