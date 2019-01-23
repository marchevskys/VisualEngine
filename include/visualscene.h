#ifndef VISUALSCENE_H
#define VISUALSCENE_H

#include "camera.h"
#include "model.h"

#include <glm/glm.hpp>

#include <float.h>
#include <memory>
#include <vector>

template <class T>
using sptr = std::shared_ptr<T>;
template <class T>
using uptr = std::unique_ptr<T>;

namespace Visual {

class Material;
class MeshData;
class ShadowMap;
class Scene {

  public:
    Scene();
    Model *addModel(Model &&model);
    void removeModel(Model *model);

    void render(const glm::dmat4 &cameraMatrix);

    ~Scene();

  private:
    struct ViewSpaceOpaqueModel {
        //glm::mat4 m_matrix;
        uptr<Model> m_model;
        ViewSpaceOpaqueModel(uptr<Model> model) { m_model = std::move(model); }
    };

    std::vector<ViewSpaceOpaqueModel> m_models;
    uptr<ShadowMap> m_shadow;
};
} // namespace Visual
#endif // VISUALSCENE_H
