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

struct RenderSystem : public ex::System<RenderSystem> {
    void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
        es.each<PhysBody, vi::Model>([dt](ex::Entity entity, PhysBody &body, vi::Model &model) {
            model.setTransform(body.getMatrix());
        });
    }
};

Game::Game() {
    m_visualScene = std::make_unique<vi::Scene>();
    m_camera = std::make_unique<vi::Camera>(glm::vec3(10, 10, 0), glm::vec3(0, 0, 0));
    m_physWorld = std::make_unique<PhysWorld>();

    systems.add<RenderSystem>();
    systems.configure();
}

void Game::loadLevel() {

    auto sphereMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.8, 0.8, 0.8});
    auto planeMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.1, 0.1, 0.1});
    auto planeMesh = std::make_shared<vi::Mesh>(vi::MeshDataPrimitive::plane(3000.0f));

    ex::Entity sphereEntity = entities.create();
    sphereEntity.assign<vi::Model>(*m_visualScene.get(), vi::MeshPrimitive::lodSphere(), sphereMaterial);
    sphereEntity.assign<PhysBody>(*m_physWorld.get(), CollisionSphere(*m_physWorld.get(), 1.0), 1.0, vec3d(0.6, 0.6, 0.6));
    sphereEntity.component<PhysBody>()->setPos({0.0, 0.0, 10.0});

    ex::Entity planeEntity = entities.create();
    planeEntity.assign<vi::Model>(*m_visualScene.get(), planeMesh, planeMaterial);
    planeEntity.assign<PhysBody>(*m_physWorld.get(), CollisionCuboid(*m_physWorld.get(), {100000, 100000, 1.0}), 0.0, vec3d(0.0, 0.0, 0.0));
}

Game::~Game() {
    ex::ComponentHandle<PhysBody> body;
    for (ex::Entity e : entities.entities_with_components(body)) {
        e.component<PhysBody>().remove();
    }
}

void Game::update(double dt) {
    m_physWorld->update(dt);
}

void Game::render(const Visual::Renderer &renderer, Visual::IFrameBuffer &frameBuffer) {
    systems.update<RenderSystem>(0.0);
    renderer.draw(*m_visualScene, *m_camera, frameBuffer);
}

void Game::addObject() {
}
