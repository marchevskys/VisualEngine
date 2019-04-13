#ifndef RENDERER_H
#define RENDERER_H
#include <memory>
namespace Visual {

class Camera;
class Scene;
class IFrameBuffer;

class Renderer {
    std::unique_ptr<class RenderData> m_renderData;
    bool m_imGuiEnabled;
  public:
    Renderer();
    ~Renderer();
    void draw(const Scene &scene, Camera &camera, const IFrameBuffer &frameBuffer) const;
};
} // namespace Visual
#endif // RENDERER_H
