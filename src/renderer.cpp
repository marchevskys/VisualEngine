#include "renderer.h"
#include "camera.h"
#include "framebuffer.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "scene.h"
#include "shader.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <mutex>
static std::mutex RenderLocker;

namespace Visual {

class RenderData {

  public:
    TextureFrameBuffer shadowTexture;
    DepthFrameBuffer shadowDepthBuffer;
    RenderData() : shadowTexture(1024), shadowDepthBuffer(shadowTexture.getId()) {
        shadowDepthBuffer.setBlendState(IFrameBuffer::Blend::Disabled);
        shadowDepthBuffer.setCullMode(IFrameBuffer::Cull::Back);
    }
};

void Renderer::draw(const Scene &scene, Camera &camera, const IFrameBuffer &window) {
    std::lock_guard<std::mutex> lock(RenderLocker); // lock the thread to avoid multi-thread shader handling

    glm::vec3 lightDir{glm::normalize(glm::vec3(1, 1, 1))};
    glm::vec4 lightDir4{lightDir, 0};

    //draw shadow
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-3.0f, 3.0f, -3.0f, 3.0f, -100.0f, 100.0f);
    glm::mat4 depthViewMatrix = glm::lookAt(lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 depthMV = depthProjectionMatrix * depthViewMatrix;

    glm::mat4 proj(1);
    // scale it to Z
    // enlarge to fit a sphere
    //
    auto eyePos = camera.getPos();
    auto viewDir = camera.getDir();
    glm::mat4 lightView = glm::lookAt(lightDir, eyePos, viewDir);
    lightView = glm::scale(lightView, glm::vec3(8));
    //    depthMV = camera.getProjection() * lightView;

    //const V3 up = camera.ge;

    //depthMV = glm::mat4(1); // camera.getProjection() * depthViewMatrix;

    glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);
    glm::mat4 depthBiasVP = biasMatrix * depthMV;
    // draw everything with shadow shader;
    auto shadowShader = ShaderShadow::get();
    shadowShader->use();
    shadowShader->setViewProjection(value_ptr(depthMV));

    auto &shadowBuffer = m_renderData->shadowDepthBuffer;
    shadowBuffer.bind();
    shadowBuffer.clear();

    scene.m_data->forEvery([&](const Scene::ShaderGroup &shaderGroup) {
        shaderGroup.forEvery([&](const Scene::MeshGroup &meshGroup) {
            meshGroup.arg->bind();
            meshGroup.forEvery([&](const Scene::MaterialGroup &materialGroup) {
                materialGroup.forEvery([&](const Model *m) {
                    shadowShader->setModel(m->getTransform());
                    m->getMesh()->draw();
                });
            });
        });
    });

    // bind window
    camera.setAR(window.bind());
    window.clear();

    // retransform shadow projection

    // draw to screen
    auto &shadowTex = m_renderData->shadowTexture;
    scene.m_data->forEvery([&](const Scene::ShaderGroup &shaderGroup) { // make nested loops great again (@Nikolay)
        const Shader3d *s = shaderGroup.arg;
        s->use();
        s->setView(value_ptr(camera.getView()));
        s->setProjection(value_ptr(camera.getProjection()));
        s->setViewPos(value_ptr(camera.getPos()));
        s->setLightDir(value_ptr(lightDir4));

        s->setShadowMatrix(value_ptr(depthBiasVP));

        shadowTex.bind(1);
        s->setShadowMap(shadowTex.getId());

        shaderGroup.forEvery([&](const Scene::MeshGroup &meshGroup) {
            meshGroup.arg->bind();
            meshGroup.forEvery([&](const Scene::MaterialGroup &materialGroup) {
                materialGroup.arg->apply();
                materialGroup.forEvery([&](const Model *m) {
                    s->setModel(m->getTransform());
                    m->getMesh()->draw();
                });
            });
        });
    });

    auto screenQuadShader = ShaderScreenQuad::get();
    screenQuadShader->use();

    shadowTex.bind(1);
    screenQuadShader->setTexture(shadowTex.getId());
    IFrameBuffer::bindDepthTest(IFrameBuffer::DepthTest::Disabled);
    Mesh::Quad()->bindAndDraw();
}

Renderer::Renderer() {
    m_renderData.reset(new RenderData);
}

Renderer::~Renderer() {
}

} // namespace Visual
