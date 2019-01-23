#include "visualscene.h"
#include "camera.h"
#include "logger.h"
#include "shadermanager.h"
#include "shadowmap.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <mutex>
std::mutex RenderLocker;

#define GLM_DMAT_CAST reinterpret_cast<glm::dmat4 &>
#define GLM_MAT_CAST reinterpret_cast<glm::mat4 &>
#define IDENTITY \
    { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 }
namespace Visual {

Scene::Scene() {
    m_shadow = std::make_unique<ShadowMap>();
}

Model *Scene::addModel(Model &&model) {
    auto uniqueModel = std::make_unique<Model>(std::move(model));
    auto modelptr = uniqueModel.get();
    m_models.emplace_back(std::move(uniqueModel));
    return modelptr;
}

void Scene::removeModel(Model *model) {
    for (auto it = m_models.begin(); it != m_models.end(); ++it) {
        if (model == it->m_model.get()) {
            m_models.erase(it);
            break;
        }
    }
}

void Scene::render(const glm::dmat4 &cameraMatrix) { // TODO: multithread matrix multiplication
    std::lock_guard<std::mutex> lock(RenderLocker);  // lock the thread to avoid multi-thread shader handling
    float near_plane = 1.0f, far_plane = 17.5f;
    static float angle = 0;
    glm::vec3 dirOfLS = glm::vec3(0.0f, 4.0f, 2.0f);
    dirOfLS = glm::rotate(dirOfLS, angle += 0.03f, glm::vec3(0, 0, 1));
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(dirOfLS, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    ShaderManager::get()->setShadowMatricesForAllShaders(glm::value_ptr(lightSpaceMatrix));
    m_shadow->bindForRenderToShadowMap();
    for (const auto &m : m_models) { // render them
        m.m_model->renderNoMaterial();
    }

    m_shadow->bindScreen();

    for (const auto &m : m_models) { // render them
        m.m_model->render();
    }
}

Scene::~Scene() {
}

} // namespace Visual
