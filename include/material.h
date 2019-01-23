#ifndef MATERIAL_H
#define MATERIAL_H
#include <memory>
namespace Visual {

class Shader;
class Texture;

struct Color {
    float r, g, b;
    Color(float _r = 0.5f, float _g = 0.5f, float _b = 0.5f) : r(_r), g(_g), b(_b) {}
    const float *get() const { return &r; }
};
class Material {
    friend class Model;
    const Shader *m_shader;
    std::shared_ptr<Texture> m_texture;
    bool m_isTransparent;

  public:
    Color m_color;
    Material();
    Material(std::shared_ptr<Texture> tex);
    Material(Color col);
    std::shared_ptr<Texture> getTexture();
    void use();
};

} // namespace Visual
#endif // MATERIAL_H
