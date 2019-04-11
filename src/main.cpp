
#include "camera.h"
#include "control.h"
#include "game.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "meshdata.h"
#include "meshloader.h"
#include "model.h"
#include "renderer.h"
#include "scene.h"
#include "texture.h"
#include "window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cassert>
#include <memory>

using namespace std;
namespace vi = Visual;

int main() {
    try {
        Window window(1024, 768, "Main window", false);

        Game game;
        game.loadLevel();
        vi::Renderer renderer;

        constexpr double dt = 1.0 / 60.0;
        while (window.active()) {
            game.update(dt);
            game.render(renderer, window);
            window.refresh();
        }
    } catch (const std::exception &except) {
        std::cerr << except.what() << std::endl;
    } catch (...) {
        std::cerr << "Undefined shit happened" << std::endl;
    }
    return 0;
}
