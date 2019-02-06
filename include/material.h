#ifndef MATERIAL_H
#define MATERIAL_H
#include <memory>
namespace Visual {

class Shader3d;
class Texture;

struct Color {
    float r, g, b;
    Color(float _r = 0.5f, float _g = 0.5f, float _b = 0.5f) : r(_r), g(_g), b(_b) {}
    const float *get() const { return &r; }
};
class IMaterial {
  protected:
    const Shader3d *m_shader;

  public:
    inline const Shader3d *getShader() const { return m_shader; }
    virtual ~IMaterial(){};
    virtual void apply() const = 0;
};

class MaterialPBR : public IMaterial {
  public:
    Color m_color;
    float m_metallic, m_roughness;
    MaterialPBR(Color c = Color());
    ~MaterialPBR(){};
    void apply() const final;
};

} // namespace Visual
#endif // MATERIAL_H
