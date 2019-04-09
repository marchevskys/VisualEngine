
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

        DLOG("main loop");
        Game game;
        game.loadLevel();

        vi::Renderer renderer;
        constexpr double dt = 1.0 / 60.0;
        while (window.active()) {

            game.update(dt);
            game.render(renderer, window);
            //            glm::dvec3 vec = {1, 0, 0};
            //            {
            //                xx += Control::mousePos().x * 0.02;
            //                yy += Control::mousePos().y * 0.02;
            //                yy = yy > M_PI_2 ? M_PI_2 - 0.001 : yy;
            //                yy = yy < -M_PI_2 ? -M_PI_2 + 0.001 : yy;
            //                vec = glm::rotate(vec, yy, glm::dvec3(0, 1, 0));
            //                vec = glm::rotate(vec, xx, glm::dvec3(0, 0, 1));
            //                distance *= 1.0 + Control::scrollOffset() * 0.07;
            //                distance = glm::clamp(distance, 0.5, 1000.0);
            //            }

            //            glm::dvec3 focusCenter(0.0, 0.0, 1.0);
            //            camera.set(vec * distance + focusCenter, focusCenter);
            //            renderer.draw(scene, camera, window);

            window.refresh();
        }
    } catch (const std::exception &except) {
        std::cerr << except.what() << std::endl;
    } catch (...) {
        std::cerr << "Undefined shit happened" << std::endl;
    }
    return 0;
}
