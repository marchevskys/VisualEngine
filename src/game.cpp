#include "game.h"
#include "camera.h"
#include "control.h"
#include "framebuffer.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "meshdata.h"
#include "model.h"
#include "physbody.h"
#include "renderer.h"
#include "scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Config.h"

struct RenderSystem : public ex::System<RenderSystem> {
    void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
        es.each<PhysBody, vi::Model>([dt](ex::Entity entity, PhysBody &body, vi::Model &model) {
            model.setTransform(body.getMatrix());
        });
    }
};

struct ControlSystem : public ex::System<ControlSystem> {
    std::shared_ptr<vi::Camera> m_Camera;
    ControlSystem(std::shared_ptr<vi::Camera> camera) : m_Camera(camera) {}

    void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
        auto attract = [&es](ex::Entity e1, glm::dvec3 &forceDir, double k) {
            auto comp1 = e1.component<PhysBody>();
            glm::dvec3 pos1 = comp1.get()->getPos();

            es.each<PhysBody>([&](ex::Entity e2, PhysBody &body) {
                auto comp2 = e2.component<PhysBody>();
                if (comp1 != comp2) {
                    glm::dvec3 pos2 = comp2.get()->getPos();
                    auto dist = glm::length(pos1 - pos2);
                    glm::dvec3 force = k * (pos1 - pos2) / (dist * dist * dist);

                    comp2.get()->addForce(force);
                    comp1.get()->addForce(-force);
                }
            });
        };

        es.each<Control, PhysBody>([dt, attract, this](ex::Entity entity, Control &control, PhysBody &body) {
            auto diff = Control::mousePos();
            glm::vec3 deltAngle = glm::dvec3(diff.y, diff.x, 0.0) * .01;

            glm::dvec3 forceDir(0, 0, 0);
            if (Control::pressed(Control::Button::Up))
                forceDir += glm::dvec3(0, 1, 0);
            if (Control::pressed(Control::Button::Down))
                forceDir += glm::dvec3(0, -1, 0);
            if (Control::pressed(Control::Button::Left))
                forceDir += glm::dvec3(-1, 0, 0);
            if (Control::pressed(Control::Button::Right))
                forceDir += glm::dvec3(1, 0, 0);
            if (Control::pressed(Control::Button::A))
                forceDir += glm::dvec3(0, 0, -1);
            if (Control::pressed(Control::Button::Z))
                forceDir += glm::dvec3(0, 0, 1);
            if (Control::pressed(Control::Button::Space))
                attract(entity, forceDir, 1000.0);
            auto forceDirLength = glm::length(forceDir);
            if (forceDirLength > 1.0)
                forceDir /= forceDirLength;
            forceDir *= 10.0;

            glm::mat3 cameraTransform = glm::dmat3(glm::inverse(m_Camera->getView<double>()));
            forceDir = cameraTransform * forceDir;
            body.addForce(forceDir);

            static float radius = 6.0f;
            glm::vec3 pos = body.getPos();
            glm::vec3 dir = glm::normalize(pos - m_Camera->getPos());

            auto transformedAngle = cameraTransform * deltAngle;
            if (glm::length(deltAngle))
                dir = glm::rotate(dir, glm::length(deltAngle), transformedAngle);

            radius *= 1.0 + Control::scrollOffset() * 0.07;
            radius = glm::clamp(radius, 0.5f, 1000.0f);
            m_Camera->set(pos - dir * radius, pos, glm::vec3(0, 0, 1));
        });
        if (Control::pressed(Control::Button::F1))
            Config::get()->set_option_enabled(Config::Option::ImGuiEnabled, true);
        if (Control::pressed(Control::Button::F2))
            Config::get()->set_option_enabled(Config::Option::ImGuiEnabled, false);
    }
};

Game::Game() {
    m_visualScene = std::make_unique<vi::Scene>();
    m_renderer = std::make_unique<vi::Renderer>();
    m_camera = std::make_shared<vi::Camera>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    m_camera->setFOV(1.6f);
    m_physWorld = std::make_unique<PhysWorld>();

    systems.add<RenderSystem>();
    systems.add<ControlSystem>(m_camera);
    systems.configure();
}

void Game::loadLevel() {

    auto sphereMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{1.8, 0.8, 0.8});
    auto planeMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.1, 0.1, 0.1});

    ex::Entity sphereEntity = entities.create();
    sphereEntity.assign<vi::Model>(*m_visualScene.get(), vi::MeshPrimitive::lodSphere(), sphereMaterial);
    sphereEntity.assign<PhysBody>(*m_physWorld.get(), CollisionSphere(*m_physWorld.get(), 1.0), 2.0, vec3d(0.6, 0.6, 0.6));
    sphereEntity.assign<Control>();
    sphereEntity.component<PhysBody>()->setPos({0.0, 0.0, 0.0});

    auto asteroidMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.2, 0.2, 0.2});
    for (int i = 0; i < 150; i++) {
        ex::Entity asteroidEntity = entities.create();
        asteroidEntity.assign<vi::Model>(*m_visualScene.get(), vi::MeshPrimitive::lodSphere(), asteroidMaterial);
        asteroidEntity.assign<PhysBody>(*m_physWorld.get(), CollisionSphere(*m_physWorld.get(), 1.0), 1.0, vec3d(0.3, 0.3, 0.3));
        asteroidEntity.component<PhysBody>()->setPos(glm::ballRand(25.0));
        asteroidEntity.component<PhysBody>()->setVelocity(glm::ballRand(25.0));
    }

    //    ex::Entity planeEntity = entities.create();
    //    glm::dvec3 cubeScale(10000.0, 10000.0, 1.0);
    //    auto cubemesh = std::make_shared<vi::Mesh>(vi::MeshDataPrimitive::cube(cubeScale));
    //    planeEntity.assign<vi::Model>(*m_visualScene.get(), cubemesh, planeMaterial);
    //    planeEntity.assign<PhysBody>(*m_physWorld.get(), CollisionCuboid(*m_physWorld.get(), cubeScale * 2.0), 0.0, vec3d(0.0, 0.0, -1.0));
}

Game::~Game() {
    ex::ComponentHandle<PhysBody> body;
    for (ex::Entity e : entities.entities_with_components(body)) {
        e.component<PhysBody>().remove();
    }
}

void Game::update(double dt) {
    systems.update<ControlSystem>(dt);
    static double k = 1.0;
    if (Control::pressed(Control::Button::S)) // sloooooow moooooootiooon
        k -= 0.05;
    else
        k += 0.05;
    k = glm::clamp(k, 0.1, 1.0);
    m_physWorld->update(dt * k);
}

void Game::render(Visual::IFrameBuffer &frameBuffer) {
    systems.update<RenderSystem>(0.0);
    m_renderer->draw(*m_visualScene, *m_camera, frameBuffer);
}

void Game::addObject() {
}
