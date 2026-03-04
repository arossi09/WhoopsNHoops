#pragma Once
#include "AABB.h"
#include "Drone.h"
#include "OBB.h"
#include "SoundManager.h"

namespace Physics {
bool handleCollision(const AABB &box, Drone &drone,
                     SoundManager &sm);

bool handleCollision(const AABB &box, Drone &drone, SoundManager &sm, bool wasTouching, bool &crashTriggeredThisFrame);
void clampToWorld(const AABB &worldBox, Drone &drone);
void resolveAABBCollision(const AABB &box, Drone &drone);
} // namespace Physics
