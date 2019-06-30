#ifndef GAME_H
#define GAME_H
#include <entityx/entityx.h>
#include <memory>
#include <string>
#include "LevelLoader.h"
#include "scene.h"
#include "camera.h"
#include "renderer.h"
#include "framebuffer.h"
#include "space.h"
#include "physbody.h"

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
    LevelLoader m_levelloader;
};

class GameData {
  public:
    vi::Scene visualScene;
    vi::Renderer renderer;
    std::shared_ptr<vi::Camera> camera;
    PhysWorld physWorld;
    Space space;
};

#endif // GAME_H
