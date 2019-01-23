#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include <memory>
namespace Visual {

class Shader;
class ShadowMap {

  public:
    ShadowMap();
    ~ShadowMap();
    void bindForRenderToShadowMap();
    void bindScreen();

  private:
    unsigned int depthMapFBO;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;
    Shader *m_shadowShader;
};
} // namespace Visual
#endif // SHADOWMAP_H
