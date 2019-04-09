#ifndef MODEL_H
#define MODEL_H
#include <glm/glm.hpp>
#include <memory>
namespace Visual {

class Mesh;
class IMaterial;
class Scene;

using std::shared_ptr;

class Model {
    friend class Scene;

  public:
    Model(Scene &scene, shared_ptr<const Mesh> mesh, shared_ptr<IMaterial> material);
    Model(Model &&other);
    Model(const Model &other);
    Model &operator=(const Model &other) = delete;

  public:
    shared_ptr<const Mesh> getMesh() const { return m_mesh; }
    shared_ptr<IMaterial> getMaterial() const { return m_material; }
    const glm::mat4 &getTransform() const { return m_transform; }
    void setTransform(const glm::mat4 &tr) { m_transform = tr; }

  private:
    Scene *m_scene;
    glm::mat4 m_transform;
    shared_ptr<const Mesh> m_mesh;
    shared_ptr<IMaterial> m_material;
};
} // namespace Visual
#endif // MODEL_H
