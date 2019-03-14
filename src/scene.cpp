#include "scene.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "shader.h"

#include <algorithm>
#include <functional>
#include <vector>

namespace Visual {

Scene::Scene() {
    m_data = std::make_unique<Container>();
}

Scene::Scene(Scene &&other) {
    forEveryModel([&](Model *m) { m->m_scene = this; });
}

void Scene::addModel(Model *m) {

    auto shaderIt = m_data->insert<Shader3d>(m->getMaterial()->getShader());
    auto meshIt = shaderIt->insert<Mesh>(m->getMesh());
    auto materialIt = meshIt->insert<IMaterial>(m->getMaterial());
    materialIt->vec.push_back(m);
}

void Scene::removeModel(Model *m) {
    auto shaderIt = m_data->find<Shader3d>(m->getMaterial()->getShader());
    if (shaderIt == m_data->vec.end())
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
            m_data->vec.erase(shaderIt);
    }
}

void Scene::forEveryModel(std::function<void(Model *)> func) {
    m_data->forEvery([&func](ShaderGroup &shaderGroup) {
        shaderGroup.forEvery([&func](MeshGroup &meshGroup) {
            meshGroup.forEvery([&func](MaterialGroup &materailGroup) {
                materailGroup.forEvery([&func](Model *m) { func(m); }); // yeah!
            });
        });
    });
}

void Scene::forEveryModel(std::function<void(const Model *)> func) const {
    m_data->forEvery([&func](const ShaderGroup &shaderGroup) {
        shaderGroup.forEvery([&func](const MeshGroup &meshGroup) {
            meshGroup.forEvery([&func](const MaterialGroup &materailGroup) {
                materailGroup.forEvery([&func](const Model *m) { func(m); }); // yeah!
            });
        });
    });
}
} // namespace Visual
