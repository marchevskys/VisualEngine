#include "framebuffer.h"
#include "logger.h"

#include <GL/glew.h>
namespace Visual {

float IFrameBuffer::bind() const {
    return bind(0, 0, m_width, m_height);
}

float IFrameBuffer::bind(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY) const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId);
    glViewport(leftTopX, leftTopY, rightBottomX, rightBottomY);
    float diffX = static_cast<float>(rightBottomX - leftTopX);
    float diffY = static_cast<float>(rightBottomY - leftTopY);
    return diffX / diffY;
}

float IFrameBuffer::bind(GLuint id) {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    return 1.0f;
}

IFrameBuffer::~IFrameBuffer() {
}

DepthFrameBuffer::DepthFrameBuffer(const GLuint textureId) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    glGenFramebuffers(1, &m_frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureId, 0);
    glDrawBuffer(GL_NONE); // No color output in the bound framebuffer, only depth.

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) // Always check that our framebuffer is ok
        THROW("Can't create FrameBuffer");
}

DepthFrameBuffer::~DepthFrameBuffer() {
    glDeleteFramebuffers(1, &m_frameBufferId);
}

void DepthFrameBuffer::clear() const {
    //bind();
    glClear(GL_DEPTH_BUFFER_BIT);
}

} // namespace Visual
