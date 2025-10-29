#pragma once
#include "AABB.h"
#include "Drone.h"

class SceneObject;
class PhysicsWorld {

public:
  // this function is needed to add collider to list
  void addColider(std::shared_ptr<AABB> collider);
  // this function is used as a wrapper to add colliders
  // from a scene object
  void addSceneObject(SceneObject &obj);
  // this function is needed to resolve collisions between
  // colliders
  void handleDroneCollisions(Drone &drone);

private:
  // this array holds all colliders
  std::vector<std::shared_ptr<AABB>> colliders;
};
