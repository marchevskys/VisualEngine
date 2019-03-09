#include "renderer.h"
#include "camera.h"
#include "framebuffer.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "scene.h"
#include "shader.h"
#include "shadowcascade.h"
#include "texture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <mutex>
static std::mutex RenderLocker;

namespace Visual {

class RenderData {
  public:
    ShadowCascade cascade;
};

void Renderer::draw(const Scene &scene, Camera &camera, const IFrameBuffer &window) {
    std::lock_guard<std::mutex> lock(RenderLocker); // lock the thread to avoid multi-thread shader handling

    glm::vec3 initDir(glm::normalize(glm::vec3(0, 1, 1)));
    static float rotangle = 0.0f;
    glm::vec3 lightDir(glm::rotate(initDir, rotangle += 0.005f, glm::vec3(0.5, 0, 1)));
    glm::vec4 lightDir4{lightDir, 0};

    auto shadowShader = ShaderShadow::get();
    IFrameBuffer::bindCullMode(IFrameBuffer::Cull::Front);
    auto renderShadow = [&](const glm::mat4 &viewProjection) {
        shadowShader->use();
        shadowShader->setViewProjection(viewProjection);
        scene.m_data->forEvery([&](const Scene::ShaderGroup &shaderGroup) {
            shaderGroup.forEvery([&](const Scene::MeshGroup &meshGroup) {
                meshGroup.arg->bind();
                meshGroup.forEvery([&](const Scene::MaterialGroup &materialGroup) {
                    materialGroup.forEvery([&](const Model *m) {
                        auto &modelTransform = reinterpret_cast<const glm::dmat4 &>(*m->getTransform());
                        shadowShader->setModel(modelTransform);
                        m->getMesh()->draw();
                    });
                });
            });
        });
    };
    m_renderData->cascade.prepareCascades(camera, lightDir);
    m_renderData->cascade.drawAll(renderShadow);

    //    glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);
    glm::dmat4 viewProjection = camera.getProjection() * camera.getView();
    auto linearSize = [&](const glm::dmat4 &model) {
        float objectRadius = glm::length(model[0] + model[1] + model[2]);
        glm::dvec3 pos(model[3]);
        auto dist = glm::distance(pos, camera.getPos<double>());
        return objectRadius / dist;
    };

    camera.setAR(window.bind());
    window.clear();
    //auto cascadeTexID = m_renderData->cascade.getDepthArrayTextureID();
    scene.m_data->forEvery([&](const Scene::ShaderGroup &shaderGroup) { // make nested loops great again (@Nikolay)
        const Shader3d *s = shaderGroup.arg;
        s->use();
        s->setView(camera.getView());
        s->setProjection(camera.getProjection());
        s->setViewPos(camera.getPos());
        s->setLightDir(lightDir4);
        s->setCascades(m_renderData->cascade.viewProjMatrix, m_renderData->cascade.splitDepth, SHADOW_MAP_CASCADE_COUNT);
        s->setShadowMap(0);

        shaderGroup.forEvery([&](const Scene::MeshGroup &meshGroup) {
            meshGroup.arg->bind();
            meshGroup.forEvery([&](const Scene::MaterialGroup &materialGroup) {
                materialGroup.arg->apply();
                materialGroup.forEvery([&](const Model *m) {
                    int maxLodLevel = m->getMesh()->getLodCount();
                    auto &modelTransform = reinterpret_cast<const glm::dmat4 &>(*m->getTransform());
                    double screenSize = linearSize(modelTransform);
                    int lod = int(maxLodLevel - sqrt(screenSize) * 10.1);
                    s->setModel(modelTransform);
                    m->getMesh()->draw(glm::clamp(lod, 0, maxLodLevel - 1));
                });
            });
        });
    });

    //    auto screenQuadShader = ShaderScreenQuad::get();
    //    screenQuadShader->use();
    //    //ITexture::bind(cascadeTexID, 0);
    //    screenQuadShader->setTexture(0);
    //    //screenQuadShader->setTexture(m_renderData->cascade.getDepthArrayTextureID());
    //    IFrameBuffer::bindDepthTest(IFrameBuffer::DepthTest::Disabled);
    //    MeshPrimitive::quad().bindAndDraw();
}

Renderer::Renderer() {
    m_renderData.reset(new RenderData);
}

Renderer::~Renderer() {
}

} // namespace Visual
