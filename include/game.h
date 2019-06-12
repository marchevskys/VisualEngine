#ifndef GAME_H
#define GAME_H
#include <entityx/entityx.h>
#include <memory>
#include <string>

namespace Visual {
class Scene;
class Camera;
class Renderer;
class IFrameBuffer;
} // namespace Visual
namespace ex = entityx;
namespace vi = Visual;

class PhysWorld;

class Game : public ex::EntityX {
  public:
    Game();
    void loadLevel();
    ~Game();

    void update(double dt);
    void render(vi::IFrameBuffer &frameBuffer);

    ex::Entity addObject(std::string command);

  private:
    std::unique_ptr<class GameData> m_data;
};

#endif // GAME_H
