#include "model.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "scene.h"

#include <glm/glm.hpp>
#include <vector>

namespace Visual {

Model &Model::operator=(const Model &other) {
    return assign(other);
}

Model &Model::assign(const Model &other) {
    if (this == &other) {
        return *this;
    }
    m_mesh = other.m_mesh;
    m_material = other.m_material;
    m_transform = other.m_transform;
    m_scene = other.m_scene;
    return *this;
}

Model::Model(Scene *scene, std::shared_ptr<Mesh> mesh, std::shared_ptr<IMaterial> material, double *transform) {
    m_mesh = mesh;
    m_material = material;
    m_transform = transform;
    scene->addModel(this);
}

template <class MaterialType>
Model::Model(Scene *scene, Mesh &&mesh, MaterialType &&material, double *transform) {
    m_mesh = std::make_shared<Mesh>(std::move(mesh));
    m_material = std::make_shared<MaterialType>(std::move(material));
    m_transform = transform;
    scene->addModel(this);
}

Model::Model(Model &&old) {
    assign(old);
    m_scene->moveModel(&old, this);
}

Model::Model(const Model &other) {
    assign(other);
    m_scene->addModel(this);
}

} // namespace Visual
