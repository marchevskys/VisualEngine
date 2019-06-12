#include "game.h"
#include "camera.h"
#include "control.h"
#include "framebuffer.h"
#include "logger.h"
#include "material.h"
#include "mesh.h"
#include "meshdata.h"
#include "model.h"
#include "octree.h"
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
#include "imgui_helper.h"

class Space {
  public:
    struct Container {
        Container(int) {}
        Container() = default;
    };

    struct Transform {
      public:
        Transform() : voxel(0), matrix(1) {}
        Transform(const glm::dvec3 &pos) : matrix(1) {
            auto vox = glm::floor((pos + halfBoxSize) / boxSize);
            voxel = static_cast<glm::ivec3>(vox);
            glm::vec3 offset = pos - vox * boxSize;
            matrix = glm::translate(matrix, offset);
        }

        Transform(const glm::dmat4 &dmat) {
            glm::dvec3 pos = dmat[3];
            auto vox = glm::floor((pos + halfBoxSize) / boxSize);
            voxel = static_cast<glm::ivec3>(vox);
            glm::vec3 offset = pos - vox * boxSize;
            matrix = static_cast<glm::mat4>(dmat);
            matrix[4] = glm::vec4(offset, dmat[3][3]);
        };

        void print() {
            std::cout << "Voxel: " << glm::to_string(voxel)
                      << "  Local: " << glm::to_string(matrix) << std::endl;
        }

        static Transform toLocal(const glm::dmat4 &mat) {
            return Transform(mat);
        };

        static glm::dmat4 toGlobal(const Transform &tr) {
            glm::dmat4 res(tr.matrix);
            res = glm::translate(res, glm::dvec3(tr.voxel) * Space::boxSize);
            return res;
        };

        glm::ivec3 voxel;
        glm::mat4 matrix;
    };

    static constexpr glm::dvec3 boxSize = glm::dvec3(100.);
    static constexpr glm::dvec3 halfBoxSize = glm::dvec3(50.);

  private:
    Octree<Container> m_tree;
};

class GameData {
  public:
    vi::Scene visualScene;
    vi::Renderer renderer;
    std::shared_ptr<vi::Camera> camera;
    PhysWorld physWorld;
    Space space;
};

struct RenderSystem : public ex::System<RenderSystem> {
    void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
        es.each<Space::Transform, vi::Model>([dt](ex::Entity entity, Space::Transform &tr, vi::Model &model) {

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
            if (Control::pressed(Control::Button::Enter)) {
                body.setVelocity(glm::vec3(0.));
                body.setOmega(glm::vec3(0.));
            }

            auto forceDirLength = glm::length(forceDir);
            if (forceDirLength > 1.0)
                forceDir /= forceDirLength;
            forceDir *= 10.0;

            vi::CameraRotateOmniDirect *cam = dynamic_cast<vi::CameraRotateOmniDirect *>(m_Camera.get());

            glm::mat3 cameraTransform = glm::dmat3(glm::inverse(cam->getView<double>()));
            forceDir = cameraTransform * forceDir;
            body.addForce(forceDir);

            float radius = cam->getDistance();
            glm::vec3 pos = body.getPos();

            radius *= 1.0 + Control::scrollOffset() * 0.07;
            radius = glm::clamp(radius, 0.5f, 1000.0f);
            auto diff = Control::mousePos();
            cam->update(pos, diff.x * 0.01f, -diff.y * 0.01f, radius);
        });

        if (Control::pressed(Control::Button::F1))
            Config::get()->set_option_value(Config::Option::ImGuiEnabled, true);
        if (Control::pressed(Control::Button::F2))
            Config::get()->set_option_value(Config::Option::ImGuiEnabled, false);
    }
};

Game::Game() {
    m_data = std::make_unique<GameData>();

    m_data->camera = std::make_shared<vi::CameraRotateOmniDirect>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    m_data->camera->setFOV(1.6f);

    systems.add<RenderSystem>();
    systems.add<ControlSystem>(m_data->camera);
    systems.configure();
}

void Game::loadLevel() {

    auto sphereMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{1.8, 0.8, 0.8});
    ex::Entity sphereEntity = entities.create();

    sphereEntity.assign<vi::Model>(m_data->visualScene, vi::MeshPrimitive::lodSphere(), sphereMaterial);
    sphereEntity.assign<PhysBody>(m_data->physWorld, CollisionSphere(m_data->physWorld, 1.0), 2.0, vec3d(0.6, 0.6, 0.6));
    sphereEntity.assign<Control>();
    vec3d shipPosition = std::any_cast<vec3d>(Config::get()->get_option(Config::Option::ShipPosition));
    sphereEntity.component<PhysBody>()->setPos(shipPosition);

    //    auto asteroidMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.2, 0.2, 0.2});
    //    for (int i = 0; i < 50; i++) {
    //        ex::Entity asteroidEntity = entities.create();
    //        asteroidEntity.assign<vi::Model>(m_data->visualScene, vi::MeshPrimitive::lodSphere(), asteroidMaterial);
    //        asteroidEntity.assign<PhysBody>(m_data->physWorld, CollisionSphere(m_data->physWorld, 1.0), 1.0, vec3d(0.3, 0.3, 0.3));
    //        asteroidEntity.component<PhysBody>()->setPos(glm::ballRand(25.0));
    //    }
    systems.update<RenderSystem>(0.0);
}

Game::~Game() {
    ex::ComponentHandle<PhysBody> body;
    for (ex::Entity e : entities.entities_with_components(body)) {
        e.component<PhysBody>().remove();
    }
}

void Game::update(double dt) {
    systems.update<ControlSystem>(dt);
    m_data->physWorld.update(dt);

    entities.each<PhysBody, Control>([](ex::Entity e, PhysBody &pb, Control &) {
        vec3d position = pb.getPos();
        ImGuiHelper::setText(glm::to_string(position));
    });

    entities.each<PhysBody, vi::Model>([dt](ex::Entity entity, PhysBody &body, vi::Model &model) {
        if (!body.isSleeping())
            model.setTransform(body.getMatrix());
    });
}

void Game::render(Visual::IFrameBuffer &frameBuffer) {
    systems.update<RenderSystem>(0.0);
    m_data->renderer.draw(m_data->visualScene, *m_data->camera, frameBuffer);
}

void Game::addObject() {
}
