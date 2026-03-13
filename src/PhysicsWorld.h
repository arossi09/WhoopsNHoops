// this class is needed to handle the physics in the world.
// It holds a list of colliders which it detects collision with
// the given SceneObject, which definiton can be found in the Scene.h file
#pragma once
#include "AABB.h"
#include "Drone.h"
#include "SoundManager.h"

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
  void handleDroneCollisions(Drone &drone, SoundManager &sm);

private:
  // this array holds all colliders
  std::vector<std::shared_ptr<AABB>> colliders;
};
