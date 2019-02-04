#include "scene.h"
#include "camera.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <algorithm>
#include <functional>
#include <mutex>

static std::mutex RenderLocker;

#define GLM_DMAT_CAST reinterpret_cast<glm::dmat4 &>
#define GLM_MAT_CAST reinterpret_cast<glm::mat4 &>
#define IDENTITY \
    { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 }
namespace Visual {

class ModelContainer {
  private:
    friend class Scene;
    std::vector<Model *> m_models;
    struct ModelComparator {
        bool operator()(const Model *m1, const Model *m2) {
            //bool shaderLess = m1->m_material->m_shader < m2->m_material->m_shader;
            //bool meshLess = m1->m_mesh < m2->m_mesh;
            //bool materialLess = m1->m_material < m2->m_material;
            //return shaderLess || meshLess || materialLess;
        }
    };

  public:
    void sort() {
        std::sort(m_models.begin(), m_models.end(), ModelComparator());
    }
    void insert(Model *m) {
        m_models.push_back(m);
        sort();
    };
    void remove(Model *m) {
        auto it = std::find(m_models.begin(), m_models.end(), m);
        m_models.erase(it);
    };
    void move(Model *old, Model *current) {
        auto it = std::find(m_models.begin(), m_models.end(), old);
        *it = current;
    }
    void forEveryModel(std::function<void(Model *)> func) {
        for (auto &m : m_models)
            func(m);
    }
};

Scene::Scene() {
}

Scene::Scene(Scene &&other) {
    m_container->forEveryModel([&](Model *m) { m->m_scene = this; });
}

void Scene::removeModel(Model *model) {
    m_container->remove(model);
}

void Scene::moveModel(Model *oldModel, Model *newModel) {
}

void Scene::sortModels() {
    m_container->sort();
}

void Scene::addModel(Model *model) {
    model->m_scene = this;
    m_container->insert(model);
}

void Scene::render(Camera &camera) {
    std::lock_guard<std::mutex> lock(RenderLocker); // lock the thread to avoid multi-thread shader handling

    for (auto &m : m_container.get()->m_models) {
        //m->render();
    }
}

Scene::~Scene() {
}

void Scene::sortVector() {
}

} // namespace Visual
