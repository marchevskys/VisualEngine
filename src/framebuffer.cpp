#include "framebuffer.h"
#include "logger.h"

#include <GL/glew.h>
namespace Visual {

float IFrameBuffer::bind() const {
    return bind(0, 0, m_width, m_height);
}

float IFrameBuffer::bind(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY) const {

    switch (m_cullFace) {
    case Cull::Front:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case Cull::Back:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    case Cull::None:
        glDisable(GL_CULL_FACE);
        break;
    }

    switch (m_blendState) {
    case Blend::Disabled:
        glDisable(GL_BLEND);
        break;
    case Blend::OneMinusSourceAlpha:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case Blend::Additive:
        THROW("is not ready yet");
        break;
    }

    switch (m_DepthTest) {
    case DepthTest::Enabled:
        glEnable(GL_DEPTH_TEST);
        break;
    case DepthTest::Disabled:
        glDisable(GL_DEPTH_TEST);
        break;
    }

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
    glGenFramebuffers(1, &m_frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferId);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureId, 0);
    glDrawBuffer(GL_NONE); // No color output in the bound framebuffer, only depth.

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) // Always check that our framebuffer is ok
        THROW("Can't create FrameBuffer");
}

DepthFrameBuffer::~DepthFrameBuffer() {
    glDeleteFramebuffers(1, &m_frameBufferId);
}

} // namespace Visual
