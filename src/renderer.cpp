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
#include <mutex>
static std::mutex RenderLocker;

namespace Visual {

class RenderData {

  public:
    TextureFrameBuffer tex;
    DepthFrameBuffer buf;
    RenderData() : tex(1024), buf(tex.getId()) {
    }
};

void Renderer::draw(const Scene &scene, Camera &camera, const IFrameBuffer &window) {
    std::lock_guard<std::mutex> lock(RenderLocker); // lock the thread to avoid multi-thread shader handling

    glm::vec3 lightDir{glm::normalize(glm::vec3(1, 1, 1))};
    glm::vec4 lightDir4{lightDir, 0};

    //draw shadow
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10.0f, 10, -10, 10, -10, 10);
    glm::mat4 depthViewMatrix = glm::lookAt(lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 depthMV = depthProjectionMatrix * depthViewMatrix;

    // bind shadow buffer
    m_renderData->buf.bind();
    // draw everything with shadow shader;
    auto shadowShader = ShaderShadow::get();
    shadowShader->use();
    scene.m_data->forEvery([&](const Scene::ShaderGroup &shaderGroup) {
        shaderGroup.forEvery([&](const Scene::MeshGroup &meshGroup) {
            meshGroup.arg->bind();
            meshGroup.forEvery([&](const Scene::MaterialGroup &materialGroup) {
                materialGroup.forEvery([&](Model *m) {
                    shadowShader->setModel(m->getTransform());
                    m->getMesh()->draw();
                });
            });
        });
    });

    // bind window
    camera.setAspectRatio(window.bind());
    //IFrameBuffer::bind(1);
    // draw to screen

    scene.m_data->forEvery([&](const Scene::ShaderGroup &shaderGroup) { // make nested loops great again (@Nikolay)
        const Shader3d *s = shaderGroup.arg;
        s->use();
        s->setView(value_ptr(camera.getView()));
        s->setProjection(value_ptr(camera.getProjection()));
        s->setViewPos(value_ptr(camera.getPos()));
        s->setLightDir(value_ptr(lightDir4));

        shaderGroup.forEvery([&](const Scene::MeshGroup &meshGroup) {
            meshGroup.arg->bind();
            meshGroup.forEvery([&](const Scene::MaterialGroup &materialGroup) {
                materialGroup.arg->apply();
                materialGroup.forEvery([&](Model *m) {
                    s->setModel(m->getTransform());
                    m->getMesh()->draw();
                });
            });
        });
    });
}

Renderer::Renderer() {
    m_renderData.reset(new RenderData);
}

Renderer::~Renderer() {
}

} // namespace Visual
