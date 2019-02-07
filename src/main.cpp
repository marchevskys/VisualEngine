
#include "camera.h"
#include "control.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "meshdata.h"
#include "model.h"
#include "renderer.h"
#include "scene.h"
#include "texture.h"
#include "window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cassert>
#include <memory>
#include <numeric>
using namespace std;
namespace vi = Visual;
int main() {

    try {
        Window window(1024, 768, "Main window", false);
        window.setCullMode(vi::IFrameBuffer::Cull::Front);

        vi::Scene scene;

        //auto texture = std::make_shared<vi::Texture>("../GameTest2/textures/jupiter_diffuse.jpg");

        vi::Color sphereColor{0.6, 0.3, 0.1};
        auto sphereMaterial = std::make_shared<vi::MaterialPBR>(sphereColor);
        glm::dmat4 sphereTransform(1);
        auto sphereMesh = std::make_shared<vi::Mesh>(vi::MeshPrimitives::sphere(40, vi::MeshData::Type::VTN));
        vi::Model sphereModel(&scene, sphereMesh, sphereMaterial, glm::value_ptr(sphereTransform));

        auto sphereMaterial2 = std::make_shared<vi::MaterialPBR>(vi::Color{0.1, 0.3, 0.8});
        glm::dmat4 sphereTransform2(1);
        sphereTransform2 = glm::translate(sphereTransform2, glm::dvec3(-1, 2, 0));
        vi::Model sphereModel2(&scene, sphereMesh, sphereMaterial2, glm::value_ptr(sphereTransform2));

        vi::Color planeColor{0.1, 0.1, 0.1};
        auto planeMaterial = std::make_shared<vi::MaterialPBR>(planeColor);
        glm::dmat4 planeTransform(1);
        planeTransform = glm::translate(planeTransform, glm::dvec3(0, 0, -1));
        auto planeMesh = std::make_shared<vi::Mesh>(vi::MeshPrimitives::plane(200, vi::MeshData::Type::VTN));
        vi::Model plane(&scene, planeMesh, planeMaterial, glm::value_ptr(planeTransform));

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
                distance *= 1.0 + Control::scrollOffset() * 0.1;
            }
            //camera.setAspectRatio(window.getAspectRatio());
            camera.set(vec * distance, glm::dvec3(0.0, 0.0, 0.0));
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
