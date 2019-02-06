#include "material.h"
#include "shader.h"
#include "texture.h"

namespace Visual {

MaterialPBR::MaterialPBR(Color c) : m_color(c) {
    m_shader = ShaderPBR::get();
}

void MaterialPBR::apply() const {
    const ShaderPBR *sh = static_cast<const ShaderPBR *>(m_shader); // it is always ShaderPBR, no reason to check by dynamic_cast
    sh->setDiffuseColor(m_color.get());
}

} // namespace Visual
