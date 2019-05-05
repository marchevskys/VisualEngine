#include "renderer.h"
#include "camera.h"
#include "framebuffer.h"
#include "gl.h"
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

#include <functional>
#include <iostream>
#include <mutex>

#include "imgui_helper.h"

#include "Config.h"

static std::mutex RenderLocker;

namespace Visual {

class RenderPass {
  public:
    struct Params {
        GL::Cull cullMode = GL::Cull::None;
        GL::DepthTest depthTest = GL::DepthTest::Enabled;
        GL::FillMode fillMode = GL::FillMode::Line;
        GL::Blend blendState = GL::Blend::Disabled;

        void set() {
            GL::setBlendState(blendState);
            GL::setCullMode(cullMode);
            GL::setDepthTest(depthTest);
            GL::setFillMode(fillMode);
        }

        enum Type {
            CascadeShadow,
            RGB,
            BoundingBoxes
        };
    };

    RenderPass() {}
    RenderPass(std::function<void()> rFunc,
               std::unique_ptr<IFrameBuffer> buf, std::unique_ptr<ITexture> outTex,
               std::string &&name, Params params) : m_renderFunction(rFunc) {
        m_renderBuffer = std::move(buf);
        m_outputTexture = std::move(outTex);
        m_params = params;
        m_name = std::move(name);
    }

    void pass() {
        m_params.set();
        m_renderBuffer->bind();
        m_outputTexture->bind(0);
        m_renderFunction();
    };

  private:
    std::function<void()> m_renderFunction;
    std::unique_ptr<IFrameBuffer> m_renderBuffer;
    std::unique_ptr<ITexture> m_outputTexture;
    std::string m_name;
    Params m_params;
};

class RenderData {
  public:
    RenderData() {
    }

    ShadowCascade cascade;
    std::vector<RenderPass> renderPasses;
};

void Renderer::draw(const Scene &scene, Camera &camera, const IFrameBuffer &window) const {
    std::lock_guard<std::mutex> lock(RenderLocker); // lock the thread to avoid multi-thread shader handling

    static float rotangle = 0.0f;
    glm::vec3 lightDir(glm::normalize(glm::vec3(1, 1, 1)));

    // shadowPass
    {
        GL::setDepthTest(GL::DepthTest::Enabled);
        GL::setFillMode(GL::FillMode::Fill);
        GL::setCullMode(GL::Cull::Front);

        m_renderData->cascade.prepareCascades(camera, lightDir);
        auto shadowShader = ShaderShadow::get();
        auto renderShadow = [&](const glm::mat4 &viewProjection) {
            shadowShader->use();
            shadowShader->setViewProjection(viewProjection);
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
        };
        m_renderData->cascade.drawAll(renderShadow);
    }

    // RGB pass
    {
        GL::setDepthTest(GL::DepthTest::Enabled);
        GL::setFillMode(GL::FillMode::Fill);
        GL::setCullMode(GL::Cull::Back);

        //    glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);
        auto linearSize = [&](const glm::mat4 &model) {
            float objectRadius = glm::length(model[0] + model[1] + model[2]);
            glm::vec3 pos(model[3]);
            auto dist = glm::distance(pos, camera.getPos());
            return objectRadius / dist;
        };

        camera.setAR(window.bind());
        window.clear();
        scene.m_data->forEvery([&](const Scene::ShaderGroup &shaderGroup) {
            const Shader3d *s = shaderGroup.arg;
            s->use();
            s->setViewMatrix(camera.getView());
            s->setProjectionMatrix(camera.getProjection());
            s->setViewPos(camera.getPos());
            s->setLightDir(lightDir);
            s->setCascades(m_renderData->cascade.viewProjMatrix, m_renderData->cascade.splitDepth, SHADOW_MAP_CASCADE_COUNT);
            s->setShadowMap(0);
            shaderGroup.forEvery([&](const Scene::MeshGroup &meshGroup) {
                meshGroup.arg->bind();
                meshGroup.forEvery([&](const Scene::MaterialGroup &materialGroup) {
                    materialGroup.arg->apply();
                    materialGroup.forEvery([&](const Model *m) {
                        int maxLodLevel = m->getMesh()->getLodCount();

                        float screenSpaceSize = linearSize(m->getTransform());
                        int lod = int(maxLodLevel - sqrt(screenSpaceSize) * 20.1);
                        s->setModelMatrix(m->getTransform());
                        m->getMesh()->draw(glm::clamp(lod, 0, maxLodLevel - 1));
                    });
                });
            });
        });
    }
    // ImGui
    if (m_imGuiEnabled) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiHelper::drawWindow();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return;
    // BBox pass
    {
        GL::setDepthTest(GL::DepthTest::Enabled);
        GL::setFillMode(GL::FillMode::Line);
        GL::setCullMode(GL::Cull::None);

        auto &cube = MeshPrimitive::cube();
        cube->bind();
        auto solidShader = ShaderPlain::get();
        solidShader->use();
        solidShader->setViewMatrix(camera.getView());
        solidShader->setProjectionMatrix(camera.getProjection());
        scene.forEveryModel([&](const Model *m) {
            auto mesh = m->getMesh();
            glm::vec3 center(mesh->getOBB().center);
            glm::vec3 scale(mesh->getOBB().scale);
            glm::mat4 transform = glm::translate(m->getTransform(), center);
            transform = glm::scale(transform, scale);
            solidShader->setModelMatrix(transform);
            cube->draw();
        });
    }

    //    auto screenQuadShader = ShaderScreenQuad::get();
    //    screenQuadShader->use();
    //    //ITexture::bind(cascadeTexID, 0);
    //    screenQuadShader->setTexture(0);
    //    //screenQuadShader->setTexture(m_renderData->cascade.getDepthArrayTextureID());
    //    GL::bindDepthTest(GL::DepthTest::Disabled);
    //    MeshPrimitive::quad().bindAndDraw();
}

Renderer::Renderer() {
    m_renderData.reset(new RenderData);
    std::any enabled = Config::get()->get_option(Config::Option::ImGuiEnabled);
    m_imGuiEnabled = std::any_cast<bool>(enabled);
    Config::get()->add_listener(Config::Option::ImGuiEnabled, [this](std::any imgui_enabled) { m_imGuiEnabled = std::any_cast<bool>(imgui_enabled); });
}

Renderer::~Renderer() {
    Config::get()->remove_listener(Config::Option::ImGuiEnabled);
}

} // namespace Visual
