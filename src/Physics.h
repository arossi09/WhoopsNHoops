#pragma Once
#include "Drone.h"
#include "AABB.h"

namespace Physics {
    void handleCollision(const AABB& box, Drone &drone);
    void clampToWorld(const AABB& worldBox, Drone& drone);
}
