#ifndef MODEL_H
#define MODEL_H
#include <memory>

namespace Visual {

class Mesh;
class IMaterial;
class Scene;

using std::shared_ptr;

class Model {
    friend class Scene;

  public:
    Model(Scene *scene, shared_ptr<Mesh> mesh, shared_ptr<IMaterial> material, double *transform);

    Model(Model &&other);
    Model(const Model &other);
    Model &operator=(const Model &other);

  public:
    inline const IMaterial *getMaterial() const { return m_material.get(); }
    inline const Mesh *getMesh() const { return m_mesh.get(); }
    inline const double *getTransform() const { return m_transform; }

  private:
    Model &
    assign(const Model &other);
    Scene *m_scene;
    double *m_transform;
    shared_ptr<Mesh> m_mesh;
    shared_ptr<IMaterial> m_material;
};
} // namespace Visual
#endif // MODEL_H
