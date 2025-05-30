#ifndef DRONE_H
#define DRONE_H

#include <glm/glm.hpp>
#include "AABB.h"

using namespace glm;

//drone struct with attributes and update
struct Drone {
    float superRate = 0.75f;
    float rcRate    = 1.1f;
    vec3 position = vec3(0.0f, 1.0f, 0.0f); 
    vec3 previousPosition = vec3(0.0f);
    quat orientation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    vec3 velocity = vec3(0.0f);
    vec3 acceleration = vec3(0.0f);
    float mass = 250.0f;
    float camera_title_angle = 25;

    //prob move this to another struct
    float rollInput = 0.0f;
    float pitchInput = 0.0f;
    float yawInput = 0.0f;
    float throttle = 0.0;

    AABB getAABB() const {
        float halfSize = .3f;  
        return AABB(position - glm::vec3(halfSize), position + glm::vec3(halfSize));
    }

    //calculate drone physics
    void updatePosition(float dt){
        previousPosition = position;
        vec3 up = orientation * vec3(0, 1, 0);
        vec3 thrust = up * (throttle * 60000.0f); //Max thrust in N

        //prob have to fix this by balancing mass and thrust instead
        vec3 gravity = vec3(0, -90.0f, 0);
        vec3 netForce = thrust + (gravity * mass);
        acceleration = netForce/mass;


        //calculate the position through acceleration & velocity
        velocity += acceleration * dt;
        velocity *= .99f;
        position += velocity * dt;
    }

    //we need this to be able to update the drones orientation based
    //off the inputs from the controller
    void updateOrientation(float rollVel, float pitchVel, float yawVel, float deltaTime){
        // Create quaternions around local axes (apply roll -> pitch ->  yaw)
        float rollDelta  = rollVel  * deltaTime;
        float pitchDelta = pitchVel * deltaTime;
        float yawDelta   = yawVel   * deltaTime;

        glm::quat qRoll  = glm::angleAxis(rollDelta,  glm::vec3(0, 0, 1)); // local Z
        glm::quat qPitch = glm::angleAxis(pitchDelta, glm::vec3(1, 0, 0)); // local X
        glm::quat qYaw   = glm::angleAxis(yawDelta,   glm::vec3(0, 1, 0)); // local Y
        orientation = orientation * qYaw * qPitch * qRoll;
        orientation = glm::normalize(orientation);
    }
};

#endif
