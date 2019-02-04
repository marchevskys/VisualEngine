#ifndef VISUALSCENE_H
#define VISUALSCENE_H

#include "camera.h"
#include "model.h"

#include <glm/glm.hpp>

#include <float.h>
#include <memory>
#include <set>
#include <vector>

namespace Visual {

class ModelContainer;
class Scene {
    void sortModels();

  public:
    Scene();
    Scene(const Scene &other) = delete;
    Scene(Scene &&other);
    ~Scene();

    void addModel(Model *model);
    void removeModel(Model *model);
    void moveModel(Model *oldModel, Model *newModel);

    void render(Camera &camera);

  private:
    //std::vector<Model *> m_models;
    void sortVector();

    std::unique_ptr<ModelContainer> m_container;
};
} // namespace Visual
#endif // VISUALSCENE_H
