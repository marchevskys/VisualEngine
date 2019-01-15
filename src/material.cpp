#include "material.h"
#include "shader.h"
#include "shadermanager.h"
#include "texture.h"
namespace Visual {

Visual::Material::Material() {
    m_shader = ShaderManager::get()->flatShader();
    m_isTransparent = false;
}

Material::Material(std::shared_ptr<Texture> tex) : Material() {
    m_texture = tex;
}

std::shared_ptr<Texture> Material::getTexture() {
    return m_texture;
}

void Material::use() {
    m_shader->use();
    //m_shader->getLocation("ourTexture1");
    if (auto &tex = m_texture)
        tex->use();
}

} // namespace Visual
