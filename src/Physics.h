#pragma Once
#include "Drone.h"
#include "AABB.h"
#include "OBB.h"

namespace Physics {
    void handleCollision(const AABB& box, Drone &drone);
    void handleCollision(const OBB& box, Drone &drone, glm::mat4 &model);
    void clampToWorld(const AABB& worldBox, Drone& drone);
    void resolveAABBCollision(const AABB &box, Drone &drone);
}
