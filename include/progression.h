#pragma once

#include <entityx/entityx.h>
#include <chrono>
#include <list>

namespace Visual {
   class Scene;
} // namespace Visual

class PhysWorld;

namespace ex = entityx;
namespace vi = Visual;

namespace Gameplay {

   class Action {
   public:
      virtual void act(ex::EntityManager& em, vi::Scene& scene, PhysWorld& physWorld) = 0;
   };

   class CreateAsteroidAction : public Action {
   public:
      CreateAsteroidAction(int amount, float radius) : amount(amount), radius(radius) {}
      void act(ex::EntityManager& em, vi::Scene& scene, PhysWorld& physWorld) override;
   private:
      int amount;
      float radius;
   };

   enum class EnemyShipLevel : int {
      Easy,
      Simple,
      Cool,
      Hard,
      Insane
   };

   class CreateEnemyShipAction : public Action {
   public:
      CreateEnemyShipAction(int amount, EnemyShipLevel shiplevel) : amount(amount), shiplevel(shiplevel) {}
      void act(ex::EntityManager& em, vi::Scene& scene, PhysWorld& physWorld) override {}
   private:
      int amount;
      EnemyShipLevel shiplevel;
   };

   struct ProgressionTrackerItem {
      double timeSince;
      double timeBefore;
      std::list<std::unique_ptr<Action>> concreteActions;
   };

   class ProgressionManager {
   public:
      ProgressionManager();
      void loadlevel(ex::EntityManager& em, vi::Scene& scene, PhysWorld& world);
      void update(double dt, ex::EntityManager& em, vi::Scene& scene, PhysWorld& world);
   private:
      std::list<ProgressionTrackerItem> m_ProgressionItems;
      double m_time;
   };

}// namespace Gameplay
