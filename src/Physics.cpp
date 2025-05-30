#include "Physics.h"


namespace Physics{
    void handleCollision(const AABB& box, Drone &drone){
        AABB droneBox = drone.getAABB();
        //if(distance(drone.position, box->getCenter())< 5){
        if(droneBox.intersects(box)){
            vec3 delta = droneBox.getCenter() - box.getCenter();
            vec3 overlap = box.getSize() * 0.5f + droneBox.getSize() * 0.5f - abs(delta);

            // Compute approximate collision normal
            vec3 penetrationDirection;
            if (overlap.x < overlap.y && overlap.x < overlap.z)
                penetrationDirection = vec3(sign(delta.x), 0, 0);
            else if (overlap.y < overlap.z)
                penetrationDirection = vec3(0, sign(delta.y), 0);
            else
                penetrationDirection = vec3(0, 0, sign(delta.z));

            // Reflect or slide velocity
            vec3 velocityNormal = dot(drone.velocity, penetrationDirection) * penetrationDirection;
            vec3 velocityTangent = drone.velocity - velocityNormal;

            float restitution = 0.4f; // Tune for bounciness
            drone.velocity = -restitution * velocityNormal + velocityTangent;

            // Move drone out of collision
            float pushback = 0.01f;
            drone.position += penetrationDirection * pushback; 
        }
    }


    void clampToWorld(const AABB& worldBox, Drone& drone) {
        
        AABB droneBox = drone.getAABB();
        vec3 halfSize = (droneBox.max - droneBox.min) * 0.5f;

        vec3 clampedPos;
        clampedPos.x = glm::clamp(drone.position.x, worldBox.min.x + halfSize.x, worldBox.max.x - halfSize.x);
        clampedPos.y = glm::clamp(drone.position.y, worldBox.min.y + halfSize.y, worldBox.max.y - halfSize.y);
        clampedPos.z = glm::clamp(drone.position.z, worldBox.min.z + halfSize.z, worldBox.max.z - halfSize.z);

        if (clampedPos.x != drone.position.x) drone.velocity.x = 0;
        if (clampedPos.y != drone.position.y) drone.velocity.y = 0;
        if (clampedPos.z != drone.position.z) drone.velocity.z = 0;

        drone.position = clampedPos;
    }
}
