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
#include <glm/gtx/hash.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include <unordered_map>

#include "Config.h"
#include "imgui_helper.h"

class Space {
  public:
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
            matrix[3] = glm::vec4(offset, dmat[3][3]);
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

    struct Container {
        Container(int) {}
        Container() = default;
        bool empty() {
            return staticEntities.empty() && dynamicEntities.empty();
        }
        std::vector<ex::Entity> staticEntities;
        std::vector<ex::Entity> dynamicEntities;
    };

    void addEntity(ex::Entity &e, Transform pos) {
        //auto &place = m_tree(pos.voxel);
        //place.staticEntities.push_back(e);
    }

    void goOutOfContainer(glm::ivec3 pos) {
        auto found = m_tree.find(pos);
        if (found != m_tree.end()) {
            auto &container = found->second;
            container.dynamicEntities.clear();
            if (container.staticEntities.empty())
                m_tree.erase(found);
        }
    }

    static constexpr double BOXSIZE = 100.;
    static constexpr glm::dvec3 boxSize = glm::dvec3(BOXSIZE);
    static constexpr glm::dvec3 halfBoxSize = glm::dvec3(BOXSIZE / 2);

  private:
    //Octree<Container> m_tree;
    std::unordered_map<glm::ivec3, Container> m_tree;
};

class GameData {
  public:
    vi::Scene visualScene;
    vi::Renderer renderer;
    std::shared_ptr<vi::Camera> camera;
    PhysWorld physWorld;
    Space space;
};

struct POISystem : public ex::System<POISystem> {
    void update(ex::EntityManager &, ex::EventManager &, ex::TimeDelta) override {
    }
};

void Game::control() {
    auto attract = [&](ex::Entity e1, glm::dvec3 &forceDir, double k) {
        auto comp1 = e1.component<PhysBody>();
        glm::dvec3 pos1 = comp1.get()->getPos();
        entities.each<PhysBody>([&](ex::Entity e2, PhysBody &body) {
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

    auto &body = *player.component<PhysBody>();
    auto &transform = *player.component<Space::Transform>();

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
    if (Control::pressed(Control::Button::D))
        body.setOmega(glm::vec3(5.));
    if (Control::pressed(Control::Button::Space))
        attract(player, forceDir, 1000.0);
    if (Control::pressed(Control::Button::Enter)) {
        body.setVelocity(glm::vec3(0.));
        body.setOmega(glm::vec3(0.));
    }

    auto forceDirLength = glm::length(forceDir);
    if (forceDirLength > 1.0)
        forceDir /= forceDirLength;
    forceDir *= 30.0;

    vi::CameraRotateOmniDirect *cam = dynamic_cast<vi::CameraRotateOmniDirect *>(m_data->camera.get());

    glm::mat3 cameraTransform = glm::dmat3(glm::inverse(cam->getView<double>()));
    forceDir = cameraTransform * forceDir;
    body.addForce(forceDir);

    float radius = cam->getDistance();
    glm::vec3 pos = transform.matrix[3];

    radius *= 1.0 + Control::scrollOffset() * 0.07;
    radius = glm::clamp(radius, 0.5f, 1000.0f);
    auto diff = Control::mousePos();
    cam->update(pos, diff.x * 0.01f, -diff.y * 0.01f, radius);

    if (Control::pressed(Control::Button::F1))
        Config::get()->set_option_value(Config::Option::ImGuiEnabled, true);
    if (Control::pressed(Control::Button::F2))
        Config::get()->set_option_value(Config::Option::ImGuiEnabled, false);
}

Game::Game() {
    Control(); // initialize control keymap
    m_data = std::make_unique<GameData>();
    m_data->camera = std::make_shared<vi::CameraRotateOmniDirect>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    m_data->camera->setFOV(1.6f);
    //systems.add<RenderSystem>();
    systems.configure();
}

class Generator {
  public:
    Generator() = delete;

    static entityx::Entity generateShip(Game& game, const glm::vec3& position) {
       ex::Entity e = game.entities.create();
       e.assign<Space::Transform>();
       static auto sphereMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{1.8, 0.8, 0.8});
       e.assign<vi::Model>(game.m_data->visualScene, vi::MeshPrimitive::lodSphere(), sphereMaterial);
       e.assign<PhysBody>(game.m_data->physWorld, CollisionSphere(game.m_data->physWorld, 1.0),
                          2.0, vec3d(0.6, 0.6, 0.6));
       e.component<PhysBody>()->setPos(position);
       return e;
    }

    static entityx::Entity generateAsteroid(Game& game, const glm::ivec3& voxel) {
        ex::Entity e = game.entities.create();
        e.assign<Space::Transform>();
        static auto asteroidMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.2, 0.2, 0.2});
        e.assign<vi::Model>(game.m_data->visualScene, vi::MeshPrimitive::lodSphere(), asteroidMaterial);
        glm::mat4 mat(1);
        mat = glm::translate(glm::ballRand(Space::BOXSIZE));
        e.component<Space::Transform>()->matrix = mat;
        e.component<Space::Transform>()->voxel = voxel;
        return e;
    }
};

void Game::loadLevel() {
    const glm::vec3 spaceShipPosition = glm::vec3(0.);
    const glm::ivec3 currentVoxel {0, 0, 0};
    player = Generator::generateShip(*this, spaceShipPosition);
    for (int i = 0; i < 50; i++)
        Generator::generateAsteroid(*this, currentVoxel);
    systems.update_all(0.0);
}

void Game::update(double dt) {
    auto tr = player.component<Space::Transform>();
    vec3d position = tr->matrix[3];
    std::string text(glm::to_string(position) + '\n' + glm::to_string(tr->voxel));
    ImGuiHelper::setText(text);

    entities.each<Space::Transform, PhysBody>([dt](ex::Entity entity, Space::Transform &tr, PhysBody &body) {
        if (!body.isSleeping())
            tr = Space::Transform(body.getMatrix());
    });

    control();
    m_data->physWorld.update(dt);
}

void Game::render(Visual::IFrameBuffer &frameBuffer) {

    const auto &currentVoxel = player.component<Space::Transform>()->voxel;

    entities.each<Space::Transform, vi::Model>([&](ex::Entity, Space::Transform &tr, vi::Model &model) {
        auto mat = glm::translate(tr.matrix, glm::vec3((tr.voxel - currentVoxel) * 100));
        model.setTransform(mat);
    });

    m_data->renderer.draw(m_data->visualScene, *m_data->camera, frameBuffer);
}

Game::~Game() {
    ex::ComponentHandle<PhysBody> body;
    for (ex::Entity e : entities.entities_with_components(body)) {
        e.component<PhysBody>().remove();
    }
}
