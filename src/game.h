#ifndef GAME_H
#define GAME_H
#include <entityx/entityx.h>
#include <memory>

namespace ex = entityx;
class Scene;
class Camera;

class Game : public ex::EntityX
{
public:
    Game();
    ~Game();

    void update(double dt);
    void render(Camera &cam);
    void addObject();

private:
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<Camera> m_camera;
};

#endif // GAME_H
