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
  // track if the drone was intersecting the previous frame
	const bool wasTouching = drone.getTouchingCollider();
	bool touchingThisFrame = false;
	bool crashTriggeredThisFrame = false;
  //drone.setWasTouching(drone.getTouchingCollider());
  //bool collided_this_frame = false;
  for (auto &collider : colliders) {
    // if the drone collided this frame save that
    bool hit = Physics::handleCollision(*collider, drone, sm, wasTouching, crashTriggeredThisFrame);
		if(!touchingThisFrame)
			touchingThisFrame = touchingThisFrame || hit;
  }
	drone.setTouchingCollider(touchingThisFrame);
}
