
#include "progression.h"
#include "material.h"
#include "model.h"
#include "mesh.h"
#include "meshdata.h"
#include "model.h"
#include "physbody.h"
#include "control.h"
#include "Config.h"

#include <glm/gtc/random.hpp>

namespace Gameplay {

   ProgressionManager::ProgressionManager()
      : m_ProgressionItems()
   {
      // The actions are to be loaded from game level files.
      std::list<std::unique_ptr<Action>> actions;
      actions.emplace_back(std::make_unique<CreateAsteroidAction>(10, 50.));
      m_ProgressionItems.push_back({ 5., 50., std::move(actions) });

      actions.clear();
      actions.emplace_back(std::make_unique<CreateEnemyShipAction>(5, EnemyShipLevel::Easy));
      m_ProgressionItems.push_back({ 80., 120., std::move(actions) });
   }

   void ProgressionManager::loadlevel(ex::EntityManager& em, vi::Scene& scene, PhysWorld& world) {
      // initial objects are loaded here
      auto sphereMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{ 1.8, 0.8, 0.8 });
      ex::Entity sphereEntity = em.create();

      sphereEntity.assign<vi::Model>(scene, vi::MeshPrimitive::lodSphere(), sphereMaterial);
      sphereEntity.assign<PhysBody>(world, CollisionSphere(world, 1.0), 2.0, vec3d(0.6, 0.6, 0.6));
      sphereEntity.assign<Control>();
      vec3d shipPosition = Config::get()->get_option<vec3d>(Config::Option::ShipPosition);
      sphereEntity.component<PhysBody>()->setPos(shipPosition);
   }

   void ProgressionManager::update(double dt, ex::EntityManager& em, vi::Scene& scene, PhysWorld& world) {
      // time to time that update shall remove outdated objects and create new
      m_time += dt;
      if (m_ProgressionItems.empty()) return;
      auto && front = m_ProgressionItems.front();
      if (m_time > front.timeBefore) {
         front.concreteActions.clear();
         m_ProgressionItems.pop_front();
      }
      else if (m_time >= front.timeSince) {
         for (auto && action: front.concreteActions) {
            action->act(em, scene, world);
         }
         front.concreteActions.clear();
         m_ProgressionItems.pop_front();
      }
   }

   void CreateAsteroidAction::act(ex::EntityManager& em, vi::Scene& scene, PhysWorld& physWorld) {
      auto asteroidMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{ 0.2, 0.2, 0.2 });
      for (int i = 0; i < amount; i++) {
         ex::Entity asteroidEntity = em.create();
         asteroidEntity.assign<vi::Model>(scene, vi::MeshPrimitive::lodSphere(), asteroidMaterial);
         asteroidEntity.assign<PhysBody>(physWorld, CollisionSphere(physWorld, 1.0), 1.0, vec3d(0.3, 0.3, 0.3));
         asteroidEntity.component<PhysBody>()->setPos(glm::ballRand(radius));
         //asteroidEntity.component<PhysBody>()->setVelocity(glm::ballRand(25.0));
      }
   }

} // namespace Gameplay

