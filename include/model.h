#ifndef MODEL_H
#define MODEL_H
#include <memory>

namespace Visual {

class Mesh;
class IMaterial;
class Scene;

class Model {
    friend class Scene;

  public:
    Model(Scene *scene, std::shared_ptr<Mesh> mesh, std::shared_ptr<IMaterial> material, double *transform);
    Model(Scene *scene, Mesh &&mesh, IMaterial &&material, double *transform);
    Model(Model &&other);
    Model(const Model &other);
    Model &operator=(const Model &other);

  private:
    Model &assign(const Model &other);
    Scene *m_scene;
    double *m_transform;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<IMaterial> m_material;
};
} // namespace Visual
#endif // MODEL_H
