#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/glm.hpp>
#include <memory>
namespace Visual {

class Shader3d;
class ITexture;

typedef glm::vec3 Color;

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
