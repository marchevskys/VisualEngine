#include "gl.h"
#include "logger.h"
#include <GL/glew.h>

GL::Cull GL::currentCullMode;
GL::Blend GL::currentBlendMode;
GL::DepthTest GL::currentDepthTestMode;
GL::FillMode GL::currentFillMode;

void GL::setCullMode(GL::Cull cull) {
    if (cull == currentCullMode)
        return;

    switch (cull) {
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
    currentCullMode = cull;
}

void GL::setBlendState(GL::Blend blend) {
    if (blend == currentBlendMode)
        return;

    switch (blend) {
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
}

void GL::setDepthTest(GL::DepthTest depthTest) {
    if (depthTest == currentDepthTestMode)
        return;

    switch (depthTest) {
    case DepthTest::Enabled:
        glEnable(GL_DEPTH_TEST);
        break;
    case DepthTest::Disabled:
        glDisable(GL_DEPTH_TEST);
        break;
    }
}

void GL::setFillMode(GL::FillMode fillMode) {
    if (fillMode == currentFillMode)
        return;

    switch (fillMode) {
    case FillMode::Line:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case FillMode::Fill:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}
