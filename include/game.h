#ifndef GAME_H
#define GAME_H
#include <entityx/entityx.h>
#include <memory>

namespace Visual {
class Scene;
class Camera;
class Renderer;
} // namespace Visual
namespace ex = entityx;
namespace vi = Visual;
class GameData;

class Game : public ex::EntityX {
  public:
    Game();
    ~Game();

    void update(double dt);
    void render(vi::Camera &cam);
    void addObject();

  private:
    std::unique_ptr<vi::Scene> m_scene;
    std::unique_ptr<vi::Renderer> m_renderer;
    std::unique_ptr<vi::Camera> m_camera;
    std::unique_ptr<GameData> m_gameData;
};

#endif // GAME_H
