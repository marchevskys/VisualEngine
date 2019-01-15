#include "logger.h"

#include "material.h"
#include "mesh.h"
#include "model.h"
#include "texture.h"

#include "shader.h"
#include "shadermanager.h"
#include <vector>

#include <glm/glm.hpp>

namespace Visual {

Model::Model(sptr<Mesh> mesh, sptr<Material> material, double *transform) {
    m_mesh = mesh;
    m_material = material;
    m_transform = transform;
}

Model::Model(Mesh &&mesh, Material &&material, double *transform) {
    m_mesh = std::make_shared<Mesh>(std::move(mesh));
    m_material = std::make_shared<Material>(std::move(material));
    m_transform = transform;
}

void Model::render(const float *viewSpaceTransform) {
    if (true) {
        m_material->use();
        m_material->m_shader->setMat4(0, viewSpaceTransform);
        m_mesh->render();
    }
}

} // namespace Visual
