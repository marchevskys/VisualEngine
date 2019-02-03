#include "material.h"
#include "shader.h"

#include "texture.h"
namespace Visual {
static const Color black{0, 0, 0};

Visual::Material::Material() {

    m_isTransparent = false;
}

Material::Material(std::shared_ptr<Texture> tex) : Material() {
    m_texture = tex;
}

Material::Material(Color col) : Material() {
    m_color = col;
}

std::shared_ptr<Texture> Material::getTexture() {
    return m_texture;
}

void Material::use() {
    m_shader->use();
    m_shader->setVec3(10, m_color.get());
    //m_shader->setVec3(10, black.get()); // 10 is RGB color
    //    if (auto &tex = m_texture) {
    //        tex->use();
    //        m_shader->setVec3(10, black.get()); // 10 is RGB color
    //    } else {
    //        tex->unuse();
    //        m_shader->setVec3(10, m_color.get());
    //    }
}

} // namespace Visual
