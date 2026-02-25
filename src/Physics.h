#pragma Once
#include "AABB.h"
#include "Drone.h"
#include "OBB.h"
#include "SoundManager.h"

namespace Physics {
void handleCollision(const AABB &box, Drone &drone,
                     SoundManager &sm);
void handleCollision(const OBB &box, Drone &drone, glm::mat4 &model);
void clampToWorld(const AABB &worldBox, Drone &drone);
void resolveAABBCollision(const AABB &box, Drone &drone);
} // namespace Physics
