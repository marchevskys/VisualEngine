#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

namespace Visual {

class FrameBuffer {
    const unsigned int m_frameBufferId = 0;

  public:
    FrameBuffer();
    void bind();
};
} // namespace Visual

#endif // FRAMEBUFFER_H
