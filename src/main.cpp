
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

#include <chrono>
#include <memory>
#include <numeric>

using namespace std;
namespace vi = Visual;
int main() {
    try {
        Window window(1024, 768, "Main window", false);

        vi::Scene scene;
        //auto mesh = std::make_shared<vi::Mesh>(vi::MeshPrimitives::icosahedron(vi::MeshData::Type::VTN));

        auto cubeMesh = std::make_shared<vi::Mesh>(vi::MeshPrimitives::cube(10.f, 10.f, 1.f, vi::MeshData::Type::VTN));
        auto sphereMesh = std::make_shared<vi::Mesh>(vi::MeshPrimitives::sphere(40, vi::MeshData::Type::VTN));
        auto texture = std::make_shared<vi::Texture>("../GameTest2/textures/jupiter_diffuse.jpg");
        auto sphereMat = std::make_shared<vi::Material>(texture);
        auto cubeMat = std::make_shared<vi::Material>(vi::Color(0.5, 0.3, 0.1));

        glm::dmat4 sphereTransform(1);
        vi::Model m(&scene, sphereMesh, sphereMat, glm::value_ptr(sphereTransform));
        glm::dmat4 cubeTransform(1);
        cubeTransform = glm::translate(cubeTransform, glm::dvec3(0, 0, -2));
        vi::Model m2(&scene, cubeMesh, cubeMat, glm::value_ptr(cubeTransform));

        double xx = M_PI_2 - 0.01, yy = -0.5;
        double distance = 5.0;
        while (window.active()) {

            glm::dvec3 vec = {1, 0, 0};
            xx += Control::mousePos().x * 0.02;
            yy += Control::mousePos().y * 0.02;
            yy = yy > M_PI_2 ? M_PI_2 - 0.001 : yy;
            yy = yy < -M_PI_2 ? -M_PI_2 + 0.001 : yy;
            vec = glm::rotate(vec, yy, glm::dvec3(0, 1, 0));
            vec = glm::rotate(vec, xx, glm::dvec3(0, 0, 1));
            distance *= 1.0 + Control::scrollOffset() * 0.1;

            //scene.render(view);
            window.refresh();
        }
    } catch (const std::exception &except) {
        std::cerr << except.what() << std::endl;
    } catch (...) {
        std::cerr << "Undefined shit happened" << std::endl;
    }
    return 0;
}
