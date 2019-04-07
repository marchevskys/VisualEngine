
#include "camera.h"
#include "control.h"
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
        vi::Scene scene;

        //auto texture = std::make_shared<vi::Texture>("../GameTest2/textures/jupiter_diffuse.jpg");

        auto sphereMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.8, 0.8, 0.8});
        glm::mat4 sphereTransform = glm::translate(glm::mat4(1), glm::vec3(0, -1.5, 2.0));
        sphereTransform = glm::scale(sphereTransform, glm::vec3(0.7));
        vi::Model sphereModel(scene, sphereTransform, vi::MeshPrimitive::lodSphere(), sphereMaterial);

        //        vi::Color planeColor{0.1, 0.1, 0.1};
        //        auto planeMaterial = std::make_shared<vi::MaterialPBR>(planeColor);
        //        glm::mat4 planeTransform(1);
        //        planeTransform = glm::translate(planeTransform, glm::vec3(0, 0, 0));
        //        auto planeMesh = std::make_shared<vi::Mesh>(vi::MeshDataPrimitive::plane(3000.0f));
        //        vi::Model plane(scene, planeTransform, planeMesh, planeMaterial);

        vi::Camera camera(glm::dvec3(10, 10, 0), glm::dvec3(0, 0, 0));
        double xx = M_PI_2 - 0.01, yy = -0.5;
        double distance = 5.0;

        DLOG("main loop");
        vi::Renderer renderer;
        while (window.active()) {

            glm::dvec3 vec = {1, 0, 0};
            {
                xx += Control::mousePos().x * 0.02;
                yy += Control::mousePos().y * 0.02;
                yy = yy > M_PI_2 ? M_PI_2 - 0.001 : yy;
                yy = yy < -M_PI_2 ? -M_PI_2 + 0.001 : yy;
                vec = glm::rotate(vec, yy, glm::dvec3(0, 1, 0));
                vec = glm::rotate(vec, xx, glm::dvec3(0, 0, 1));
                distance *= 1.0 + Control::scrollOffset() * 0.07;
                distance = glm::clamp(distance, 0.5, 1000.0);
            }

            glm::dvec3 focusCenter(0.0, 0.0, 1.0);
            camera.set(vec * distance + focusCenter, focusCenter);
            renderer.draw(scene, camera, window);

            window.refresh();
        }
    } catch (const std::exception &except) {
        std::cerr << except.what() << std::endl;
    } catch (...) {
        std::cerr << "Undefined shit happened" << std::endl;
    }
    return 0;
}
