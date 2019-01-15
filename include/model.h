#ifndef MODEL_H
#define MODEL_H
#include <memory>

template <class T>
using sptr = std::shared_ptr<T>;
template <class T>
using uptr = std::unique_ptr<T>;

namespace Visual {

class Mesh;
class Material;
class Scene;

class Model {
    friend class Scene;
    double *m_transform;
    sptr<Mesh> m_mesh;
    sptr<Material> m_material;
    bool m_visible = true;
    void render(const float *transform); // camera space transform

  public:
    Model(sptr<Mesh> mesh, sptr<Material> material, double *transform);
    Model(Mesh &&mesh, Material &&material, double *transform);

    void setTransformPtr(double *tr) { m_transform = tr; }
    void setVisibility(bool visibility) { m_visible = visibility; }
    void setMaterial(sptr<Material> mat) { m_material = mat; }
    void setMesh(sptr<Mesh> mesh) { m_mesh = mesh; }

    void getTransormPtr(double *tr) { m_transform = tr; }
    bool getVisibility() { return m_visible; }
    sptr<Material> getMaterial() { return m_material; }
    sptr<Mesh> getMesh() { return m_mesh; }
};
} // namespace Visual
#endif // MODEL_H
