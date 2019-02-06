#include "scene.h"
#include "camera.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"

//#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
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

using glm::value_ptr;

class ModelContainer {
  public:
    //ModelContainer (){};
    friend class Scene;
    template <class Inner, class Arg>
    struct Group {
        std::vector<Inner> vec;
        const Arg *arg;
        Group(const Arg *ar) : arg(ar) {}
        Group() : arg(nullptr) {}
        ~Group(){};
        template <typename T>
        typename std::vector<Inner>::iterator find(const T *ar) {
            if (!vec.empty())
                return std::find_if(vec.begin(), vec.end(), [&ar](const Inner &v) { return ar == v.arg; });
            else
                return vec.end();
        }

        template <typename T>
        typename std::vector<Inner>::iterator insert(const T *ar) {
            auto it = find(ar); // find this shader in array
            if (it == vec.end()) {
                vec.emplace_back(Inner(ar));
                it = vec.end() - 1;
            }
            return it;
        }

        void triggerArg(std::function<void(Arg *)> func) { func(arg); }
        void forEvery(std::function<void(Inner &value)> func) {
            for (auto &v : vec)
                func(v);
        }
    };

    typedef Group<Model *, IMaterial> MaterialGroup;
    typedef Group<MaterialGroup, Mesh> MeshGroup;
    typedef Group<MeshGroup, Shader3d> ShaderGroup;
    typedef Group<ShaderGroup, void> Container;
    Container m_data;
    ~ModelContainer() {}

  public:
    void addModel(Model *m) {
        auto shaderIt = m_data.insert<Shader3d>(m->getMaterial()->getShader());
        auto meshIt = shaderIt->insert<Mesh>(m->getMesh());
        auto materialIt = meshIt->insert<IMaterial>(m->getMaterial());
        materialIt->vec.push_back(m);
    }
    void removeModel(Model *m) {
        auto shaderIt = m_data.find<Shader3d>(m->getMaterial()->getShader());
        if (shaderIt == m_data.vec.end())
            THROW("Model does not exist");
        auto meshIt = shaderIt->find<Mesh>(m->getMesh());
        if (meshIt == shaderIt->vec.end())
            THROW("Mesh does not exist");
        auto materialIt = meshIt->find<IMaterial>(m->getMaterial());
        if (materialIt == meshIt->vec.end())
            THROW("Material does not exist");
        auto modelIt = std::find(materialIt->vec.begin(), materialIt->vec.end(), m); // Model
        if (modelIt == materialIt->vec.end())
            THROW("Model does not exist");

        materialIt->vec.erase(modelIt);

        if (materialIt->vec.empty()) {
            meshIt->vec.erase(materialIt);
            if (meshIt->vec.empty())
                shaderIt->vec.erase(meshIt);
            if (shaderIt->vec.empty())
                m_data.vec.erase(shaderIt);
        }
    }
    void forEveryModel(std::function<void(Model *)> func) {
        m_data.forEvery([&func](ShaderGroup &shaderGroup) {
            shaderGroup.forEvery([&func](MeshGroup &meshGroup) {
                meshGroup.forEvery([&func](MaterialGroup &materailGroup) {
                    materailGroup.forEvery([&func](Model *m) { func(m); }); // yeah!
                });
            });
        });
    }
};

Scene::Scene() {
    m_container.reset(new ModelContainer());
}

Scene::Scene(Scene &&old) {
    m_container->forEveryModel([&](Model *m) { m->m_scene = this; });
}

void Scene::removeModel(Model *model) {
    m_container->removeModel(model);
}

void Scene::moveModel(Model *oldModel, Model *newModel) {
    THROW("Temporary unvailable");
}

void Scene::addModel(Model *model) {
    model->m_scene = this;
    m_container->addModel(model);
}

void Scene::render(Camera &camera) {
    std::lock_guard<std::mutex> lock(RenderLocker); // lock the thread to avoid multi-thread shader handling

    glm::vec4 lightDir{glm::normalize(glm::vec3(1, 1, 1)), 0};

    const auto &container = m_container.get();
    using MC = ModelContainer;

    container->m_data.forEvery([&](MC::ShaderGroup &shaderGroup) { // make nested loops great again (@Nikolay)
        const Shader3d *s = shaderGroup.arg;
        s->use();
        s->setView(value_ptr(camera.getView()));
        s->setProjection(value_ptr(camera.getProjection()));
        s->setViewPos(value_ptr(camera.getPos()));
        s->setLightDir(value_ptr(lightDir));

        shaderGroup.forEvery([&](MC::MeshGroup &meshGroup) {
            meshGroup.arg->bind();
            meshGroup.forEvery([&](MC::MaterialGroup &materialGroup) {
                materialGroup.arg->apply();
                materialGroup.forEvery([&](Model *m) {
                    s->setModel(m->m_transform);
                    m->getMesh()->draw();
                });
            });
        });
    });
}

Scene::~Scene() {
    const auto &arr = m_container.get()->m_data;
}

} // namespace Visual
