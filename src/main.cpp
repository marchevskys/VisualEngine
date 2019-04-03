
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
        //std::vector<vi::MeshData> sphereData;
        //sphereData.emplace_back(vi::MeshDataPrimitive::sphere(40));
        //sphereData.emplace_back(vi::MeshDataPrimitive::sphere(20));
        //sphereData.emplace_back(vi::MeshDataPrimitive::sphere(10));
        //sphereData.emplace_back(vi::MeshDataPrimitive::sphere(5));
        //sphereData.emplace_back(vi::MeshDataPrimitive::sphere(2));
        //auto sphereMesh = std::make_shared<vi::Mesh>(sphereData);

        //auto torusMesh = std::make_shared<vi::Mesh>(vi::MeshLoader::load("torus2.obj"));
        //auto torusMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.5, 0.02, 0.1});
        //glm::dmat4 torusTransform = glm::translate(glm::dmat4(1), glm::dvec3(0, -1.5, 0.7));
        //torusTransform = glm::scale(torusTransform, glm::dvec3(0.005));
        //vi::Model torusModel(&scene, torusMesh, torusMaterial, glm::value_ptr(torusTransform));

		auto asteroidMesh = std::make_shared<vi::Mesh>(vi::MeshLoader::load("asteroid1.obj"));
		auto asteroidMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.5, 0.02, 0.1});
		glm::dmat4 asteroidTransform = glm::translate(glm::dmat4(1), glm::dvec3(40, 1.3, 10));
		asteroidTransform = glm::scale(asteroidTransform, glm::dvec3(5.0));
		vi::Model asteroidModel(&scene, asteroidMesh, asteroidMaterial, glm::value_ptr(asteroidTransform));

        //auto sphereMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.8, 0.8, 0.8});
        //glm::dmat4 sphereTransform = glm::translate(glm::dmat4(1), glm::dvec3(0, -1.5, 2.0));
        //sphereTransform = glm::scale(sphereTransform, glm::dvec3(0.7));
        //vi::Model sphereModel(&scene, sphereMesh, sphereMaterial, glm::value_ptr(sphereTransform));

        //auto horseMesh = std::make_shared<vi::Mesh>(vi::MeshLoader::load("horse.obj"));
        //auto horseMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.15, 0.1, 0.5});
        //glm::dmat4 horseTransform = glm::translate(glm::dmat4(1), glm::dvec3(0, 1.5, 0));
        //vi::Model horseModel(&scene, horseMesh, horseMaterial, glm::value_ptr(horseTransform));

        //auto sphereMaterial3 = std::make_shared<vi::MaterialPBR>(vi::Color{0.8, 0.8, 0.8});
        //glm::dmat4 sphereTransform3 = glm::translate(glm::dmat4(1), glm::dvec3(40, 1.3, 10));
        //sphereTransform3 = glm::scale(sphereTransform3, glm::dvec3(10.0));
        //vi::Model sphereModel3(&scene, sphereMesh, sphereMaterial3, glm::value_ptr(sphereTransform3));

        //auto sphereMaterial4 = std::make_shared<vi::MaterialPBR>(vi::Color{0.3, 0.3, 0.3});
        //glm::dmat4 sphereTransform4 = glm::translate(glm::dmat4(1), glm::dvec3(-240, 1.3, 100));
        //sphereTransform4 = glm::scale(sphereTransform4, glm::dvec3(100.0));
        //vi::Model sphereModel4(&scene, sphereMesh, sphereMaterial4, glm::value_ptr(sphereTransform4));

        vi::Color planeColor{0.1, 0.1, 0.1};
        auto planeMaterial = std::make_shared<vi::MaterialPBR>(planeColor);
        glm::dmat4 planeTransform(1);
        planeTransform = glm::translate(planeTransform, glm::dvec3(0, 0, 0));
        auto planeMesh = std::make_shared<vi::Mesh>(vi::MeshDataPrimitive::plane(3000.0f));
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
