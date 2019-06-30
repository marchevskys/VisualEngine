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

#include "space.h"
#include "Generator.h"



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

Game::Game() 
    : m_levelloader(*this) 
{
    Control(); // initialize control keymap
    m_data = std::make_shared<GameData>();
    m_data->camera = std::make_shared<vi::CameraRotateOmniDirect>(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0));
    m_data->camera->setFOV(1.6f);
    //systems.add<RenderSystem>();
    systems.configure();
}

void Game::loadLevel() {
    player = m_levelloader.tryLoadLevel("levels/level1.lvl");
    //const glm::vec3 spaceShipPosition = glm::vec3(0.);
    //const glm::ivec3 currentVoxel {0, 0, 0};
    //player = Generator::generateShip(*this, spaceShipPosition);
    //for (int i = 0; i < 50; i++)
    //    Generator::generateAsteroid(*this, currentVoxel);
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

std::shared_ptr<GameData> Game::getGameData() const {
   return m_data;
}
