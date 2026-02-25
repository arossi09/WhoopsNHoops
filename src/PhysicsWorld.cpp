#include "PhysicsWorld.h"
#include "Physics.h"
#include "Scene.h"

void PhysicsWorld::addColider(std::shared_ptr<AABB> collider) {
  colliders.push_back(collider);
}

void PhysicsWorld::addSceneObject(SceneObject &obj) {
  for (auto &c : obj.colliders) {
    addColider(c);
  }
}
void PhysicsWorld::handleDroneCollisions(Drone &drone, SoundManager &sm) {
  for (auto &collider : colliders) {
    Physics::handleCollision(*collider, drone, sm);
  }
}
