#ifndef GAME_H
#define GAME_H
#include <entityx/entityx.h>
#include <memory>
#include <string>
#include "LevelLoader.h"

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
    void control();

    ex::Entity addObject(std::string command);
    std::shared_ptr<class GameData> getGameData() const;
  private:
    ex::Entity player;
    std::shared_ptr<class GameData> m_data;
};

#endif // GAME_H
