#include "model.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "scene.h"

#include <vector>

namespace Visual {

Model::Model(Scene &scene, glm::mat4 &transform, shared_ptr<Mesh> mesh, shared_ptr<IMaterial> material)
    : m_scene(&scene), m_transform(transform), m_mesh(mesh), m_material(material) {

    m_scene->addModel(this);
}

Model::Model(const Model &old) : m_scene(old.m_scene), m_transform(old.m_transform),
                                 m_mesh(old.m_mesh), m_material(old.m_material) {
    m_scene->addModel(this);
}

Model::Model(Model &&other) : m_scene(other.m_scene), m_transform(other.m_transform),
                              m_mesh(other.m_mesh), m_material(other.m_material) {
    THROW("NOT READY");
    //m_scene->moveModel(&old, this);
}

} // namespace Visual
