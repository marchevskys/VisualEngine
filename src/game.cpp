#include "game.h"
#include "camera.h"
#include "control.h"
#include "physbody.h"
#include "renderer.h"
#include "scene.h"

class GameData {
};

Game::Game() {
    m_scene = std::make_unique<vi::Scene>();
    m_camera = std::make_unique<vi::Camera>(glm::vec3(10, 10, 0), glm::vec3(0, 0, 0));
}

Game::~Game() {
}

void Game::addObject() {
}
