#include "logger.h"

#include "material.h"
#include "mesh.h"
#include "model.h"
#include "scene.h"
#include "shader.h"

#include <vector>

#include <glm/glm.hpp>

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

Model::Model(Scene *scene, sptr<Mesh> mesh, sptr<Material> material, double *transform) {
    m_mesh = mesh;
    m_material = material;
    m_transform = transform;
    scene->addModel(this);
}

Model::Model(Scene *scene, Mesh &&mesh, Material &&material, double *transform) {
    m_mesh = std::make_shared<Mesh>(std::move(mesh));
    m_material = std::make_shared<Material>(std::move(material));
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

void Model::render() {
    m_material->use();
    m_material->m_shader->setMat4(0, m_transform);
    m_mesh->render();
}

void Model::renderNoMaterial() {
    m_material->m_shader->setMat4(0, m_transform);
    m_mesh->render();
}

} // namespace Visual
