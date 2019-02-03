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
    Model &assign(const Model &other);

  public:
    Scene *m_scene;
    double *m_transform;
    sptr<Mesh> m_mesh;
    sptr<Material> m_material;

    Model(Scene *scene, sptr<Mesh> mesh, sptr<Material> material, double *transform);
    Model(Scene *scene, Mesh &&mesh, Material &&material, double *transform);
    Model(Model &&other);
    Model(const Model &other);
    Model &operator=(const Model &other);

    void render();
    void renderNoMaterial();
};
} // namespace Visual
#endif // MODEL_H
