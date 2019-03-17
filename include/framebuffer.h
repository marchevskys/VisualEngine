#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

namespace Visual {
typedef unsigned int GLuint;

class ITexture;
class IFrameBuffer {
  public:
    float bind() const;
    float bind(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY) const;

    static float bind(GLuint id);
    virtual void clear() const = 0;
    virtual ~IFrameBuffer() = 0;

  protected:
    GLuint m_frameBufferId = 0;
    int m_width = 1024, m_height = 1024;
};

class DepthFrameBuffer : public IFrameBuffer {
  public:
    DepthFrameBuffer(const GLuint textureId);
    ~DepthFrameBuffer();
    void clear() const override;
};
} // namespace Visual

#endif // FRAMEBUFFER_H
