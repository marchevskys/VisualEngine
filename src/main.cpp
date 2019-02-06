
#include "control.h"
#include "logger.h"
#include "window.h"

#include "material.h"
#include "mesh.h"
#include "meshdata.h"

#include "scene.h"
#include "texture.h"

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

        vi::Scene scene;

        auto sphereMesh = std::make_shared<vi::Mesh>(vi::MeshPrimitives::sphere(40, vi::MeshData::Type::VTN));
        //auto texture = std::make_shared<vi::Texture>("../GameTest2/textures/jupiter_diffuse.jpg");
        vi::Color c{0.1, 0.3, 0.7};
        auto mat = std::make_shared<vi::MaterialPBR>(c);

        glm::dmat4 sphereTransform(1);
        vi::Model m(&scene, sphereMesh, mat, glm::value_ptr(sphereTransform));

        vi::Camera camera(vec3d(10, 10, 0), vec3d(0, 0, 0));
        double xx = M_PI_2 - 0.01, yy = -0.5;
        double distance = 5.0;
        DLOG("main loop");
        while (window.active()) {

            glm::dvec3 vec = {1, 0, 0};
            xx += Control::mousePos().x * 0.02;
            yy += Control::mousePos().y * 0.02;
            yy = yy > M_PI_2 ? M_PI_2 - 0.001 : yy;
            yy = yy < -M_PI_2 ? -M_PI_2 + 0.001 : yy;
            vec = glm::rotate(vec, yy, glm::dvec3(0, 1, 0));
            vec = glm::rotate(vec, xx, glm::dvec3(0, 0, 1));
            distance *= 1.0 + Control::scrollOffset() * 0.1;

            camera.setAspectRatio(window.getAspectRatio());
            camera.set(vec * distance, glm::dvec3(0.0, 0.0, 0.0));
            scene.render(camera);
            window.refresh();
        }
    } catch (const std::exception &except) {
        std::cerr << except.what() << std::endl;
    } catch (...) {
        std::cerr << "Undefined shit happened" << std::endl;
    }
    return 0;
}
