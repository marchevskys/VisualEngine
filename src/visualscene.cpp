#include "visualscene.h"
#include "logger.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <mutex>
std::mutex RenderLocker;

#define GLM_DMAT_CAST reinterpret_cast<glm::dmat4 &>
#define GLM_MAT_CAST reinterpret_cast<glm::mat4 &>
#define IDENTITY \
    { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 }
namespace Visual {

Scene::Scene() {
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

    for (auto &m : m_models) {
        auto glmModelTransform = glm::make_mat4(m.m_model->m_transform);
        //m.m_matrix = cameraMatrix * glmModelTransform;
        m.m_matrix = glmModelTransform * cameraMatrix;
    }
    for (const auto &m : m_models) { // render them
        m.m_model->render(glm::value_ptr(m.m_matrix));
    }
    //THROW();
}

Scene::~Scene() {
}

} // namespace Visual
