#pragma once

#include "entityx/entityx.h"
#include "glm/glm.hpp"

class Game;

class Generator {
  public:
    Generator() = delete;

    static entityx::Entity generateShip(Game& game, const glm::vec3& position);
    static entityx::Entity generateAsteroid(Game& game, const glm::ivec3& voxel);
};