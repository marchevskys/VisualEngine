#include "Generator.h"
#include "physbody.h"
#include "game.h"
#include "model.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "space.h"

entityx::Entity Generator::generateShip(Game& game, const glm::vec3& position) {
    ex::Entity e = game.entities.create();
    e.assign<Space::Transform>();
    static auto sphereMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{ 1.8, 0.8, 0.8 });
    e.assign<vi::Model>(game.getGameData()->visualScene, vi::MeshPrimitive::lodSphere(), sphereMaterial);
    e.assign<PhysBody>(game.getGameData()->physWorld, CollisionSphere(game.getGameData()->physWorld, 1.0),
        2.0, vec3d(0.6, 0.6, 0.6));
    e.component<PhysBody>()->setPos(position);
    return e; 
}

entityx::Entity Generator::generateAsteroid(Game& game, const glm::ivec3& voxel) {
    ex::Entity e = game.entities.create();
    e.assign<Space::Transform>();
    static auto asteroidMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{ 0.2, 0.2, 0.2 });
    e.assign<vi::Model>(game.getGameData()->visualScene, vi::MeshPrimitive::lodSphere(), asteroidMaterial);
    glm::mat4 mat(1);
    mat = glm::translate(glm::ballRand(Space::BOXSIZE));
    e.component<Space::Transform>()->matrix = mat;
    e.component<Space::Transform>()->voxel = voxel;
    return e;
}
