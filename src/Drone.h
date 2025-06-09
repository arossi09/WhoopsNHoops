#ifndef DRONE_H
#define DRONE_H

#include <glm/glm.hpp>
#include "AABB.h"

using namespace glm;

//drone struct with attributes and update
struct Drone {
    float superRate = 0.61f;
    float rcRate    = 1.0f;
    float maxVelocity = 70.0f;
    vec3 position = vec3(0.0f, 1.0f, 0.0f); 
    vec3 previousPosition = vec3(0.0f);
    quat orientation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    quat prevorientation = quat(1.0, 0.0f, 0.0f, 0.0f);
    vec3 velocity = vec3(0.0f);
    vec3 acceleration = vec3(0.0f);
    float mass = 250.0f;
    float camera_title_angle = 25;

    std::string trick = "";
    int string_count = 0;
    int score = 0;

    //prob move this to another struct
    float rollInput = 0.0f;
    float pitchInput = 0.0f;
    float yawInput = 0.0f;
    float throttle = 0.0;


    //trick detector
    float rollAcum = 0.0f;
    float pitchAcum = 0.0f;
    float yawAcum = 0.0f;
    float dPitch = 0.0f;
    float dYaw= 0.0f;
    float dRoll= 0.0f;
    bool intrick = false;
    bool diving = false;
    bool diveTextAdded = false;
    float timeSinceLastTrick = 0.0f;
    float rollTimer = 0.0f;
    float pitchTimer = 0.0f;
    float yawTimer = 0.0f;
    float maxTricktime = 1.5f;

    AABB getAABB() const {
        float halfSize = .7f;  
        return AABB(position - glm::vec3(halfSize), position + glm::vec3(halfSize));
    }

    //calculate drone physics
    void updatePosition(float dt){
        previousPosition = position;
        vec3 up = orientation * vec3(0, 1, 0);
        vec3 thrust = up * (throttle * 60000.0f); //Max thrust in N

        //prob have to fix this by balancing mass and thrust instead
        vec3 gravity = vec3(0, -95.0f, 0);
        vec3 netForce = thrust + (gravity * mass);
        acceleration = netForce/mass;


        //calculate the position through acceleration & velocity
        velocity += acceleration * dt;
        velocity *= .99f;
        position += velocity * dt;

        if (length(velocity) > maxVelocity) {
            velocity = normalize(velocity) * maxVelocity;
        }
    }

    void updateTrickState(float dt){
       // we need to calculate the delta angles for pitch, yaw, and 
       // roll to see if we complete full rotations
       glm::quat deltaQ = glm::inverse(prevorientation) * orientation;
       glm::vec3 eulerDelta = glm::eulerAngles(deltaQ);

       //these hold the delta values in case we are able to add it to total
       //accum 
       dRoll= glm::degrees(eulerDelta.z);
       dPitch= glm::degrees(eulerDelta.x);
       dYaw= glm::degrees(eulerDelta.y);
       timeSinceLastTrick += dt; 
       static std::string last_trick = "";

       if(timeSinceLastTrick > 8){
            score = 0;
            trick = "";
            string_count = 0;
            timeSinceLastTrick = 0; 
       }

       if(string_count > 3){
           string_count = 1;
           trick = last_trick; 
       }


       //these ensure threshold for the trick to be done within
       //the maxTricktime limit
       //
       //time starts once the delta is over 0.5
       if(abs(dRoll)> 0.5f){
         rollTimer += dt;
         rollAcum += dRoll;
       }


       if(abs(dPitch)> 0.5f){
         pitchTimer+= dt;
         pitchAcum += dPitch;
       }


       if(abs(dYaw)> 0.5f){
         yawTimer+= dt;
         yawAcum += dYaw;
       }

       //this checks if the delta in each axis is a full rotation 
       //and the timer for each axis has not exceeded the maxTricktime
       if(abs(rollAcum) >= 320.0f && rollTimer <= maxTricktime){
           score += 200;
           string_count += 1;
           if(string_count > 1){
               trick += " + ";
           }
           trick += "barrel roll!";
           last_trick = "barrel roll!";
           rollAcum = 0.0f;
           rollTimer = 0.0f;
           timeSinceLastTrick = 0.0f;
       }//if we exceed the maxTrick time reset the time and total delta
       else if (rollTimer > maxTricktime) { 
        rollAcum = 0.0f;
        rollTimer = 0.0f;
        }

       if(abs(pitchAcum) >= 320.0f && pitchTimer <= maxTricktime){
           score += 400;
           string_count += 1;
           if(string_count > 1){
                trick += " + ";
           }
           trick += "Front/Back flip!";
           pitchAcum= 0.0f;
           pitchTimer = 0.0f;
           timeSinceLastTrick = 0.0f;
       }else if(pitchTimer > maxTricktime){
           pitchTimer = 0.0f;
           pitchAcum = 0.0f;
       }

       if(abs(yawAcum) >= 360.0f && yawTimer <= maxTricktime){
           score += 100;
           string_count += 1;
           if(string_count > 1){
                trick += " + ";
           }
           trick += "Yaw Spin!";
           last_trick = "Yaw Spin!";
           yawAcum = 0.0f;
           yawTimer = 0.0f;
           timeSinceLastTrick = 0.0f;
       }else if(yawTimer > maxTricktime){
           yawTimer = 0.0f;
           yawAcum = 0.0f;
       }

       if(velocity.y < -40){
           if(!diving){
               diving = true;
                diveTextAdded = false;
           }
           score += 10;

           if(!diveTextAdded){
               string_count += 1;
               if(string_count > 1){
                    trick += " + ";
               }
               trick += "Dive!";
               last_trick = "Dive!";
               timeSinceLastTrick = 0.0f;
               diveTextAdded = true;
           }
       }else{
           diving = false;
           diveTextAdded = false;
       }
       prevorientation = orientation;
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




    void updateMouseOrientation(float phi, float theta, float dt){
        glm::quat qPitch = glm::angleAxis(glm::radians(phi), glm::vec3(1, 0, 0));   // pitch around X
        glm::quat qYaw   = glm::angleAxis(glm::radians(theta), glm::vec3(0, 1, 0)); // yaw around Y

        orientation = qYaw * qPitch;

        orientation = glm::normalize(orientation);}
};

#endif
