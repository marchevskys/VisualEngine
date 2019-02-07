#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

namespace Visual {
typedef unsigned int GLuint;

class ITexture;
class IFrameBuffer {
  public:
    // clang-format off
    enum class Cull { Front, Back, None } m_cullFace = Cull::None;
    enum class Blend { Disabled, OneMinusSourceAlpha, Additive } m_blendState = Blend::Disabled;
    enum class DepthTest { Enabled, Disabled } m_DepthTest = DepthTest::Enabled;
    // clang-format on

    float bind() const;
    float bind(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY) const;
    static float bind(GLuint id);
    void setCullMode(Cull cull) { m_cullFace = cull; }
    void setBlendState(Blend blend) { m_blendState = blend; }
    void setDepthTest(DepthTest dt) { m_DepthTest = dt; }
    virtual ~IFrameBuffer() = 0;

  protected:
    GLuint m_frameBufferId = 0;
    int m_width = 256, m_height = 256;
};

class DepthFrameBuffer : public IFrameBuffer {
  public:
    DepthFrameBuffer(const GLuint textureId);
    ~DepthFrameBuffer();
};
} // namespace Visual

#endif // FRAMEBUFFER_H
