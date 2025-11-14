#ifndef DRONE_H
#define DRONE_H

#include "AABB.h"
#include "TrickManager.h"
#include <glm/glm.hpp>
#include <iostream>
#include <string>

using namespace glm;

#define DECAY_RATE .01
#define THROTTLE_FACTOR .5

enum drone_states { INVERTED, NONE, COMPLETE, TIMER_START };

template <typename T>
std::string join(const std::vector<T> &arr, const std::string &delimiter) {
  std::string result = "";
  if (!arr.empty()) {
    result += arr[0];
    for (size_t i = 1; i < arr.size(); i++) {
      result += delimiter;
      result += arr[i];
    }
  }
  return result;
}

struct Fsm {
  enum drone_states state;
  enum drone_states previous_state;
};

// drone struct with attributes and update
struct Drone {
  Fsm splitS_state;
  Fsm roll_state;
  float battery = 100.0f;
  float superRate = 0.61f;
  float rcRate = 1.0f;
  float maxVelocity = 100.0f;
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

  // prob move this to another struct
  float rollInput = 0.0f;
  float pitchInput = 0.0f;
  float yawInput = 0.0f;
  float throttle = 0.0;


  // trick detector
  TrickManager trickManager;
  float dPitch = 0.0f;
  float dYaw = 0.0f;
  float dRoll = 0.0f;
  float maxTricktime = 2;

  AABB getAABB() const {
    float halfSize = .7f;
    return AABB(position - glm::vec3(halfSize), position + glm::vec3(halfSize));
  }

  // calculate drone physics
  void updatePosition(float dt) {
    /*
    battery -= DECAY_RATE + THROTTLE_FACTOR * throttle * dt;
    battery = max(battery, 0.0f);
    if(battery> 100.0f ){
        battery = 100.0f;
    }
    */

    previousPosition = position;
    vec3 up = orientation * vec3(0, 1, 0);
    vec3 thrust = up * (throttle * 60000.0f); // Max thrust in N

    // prob have to fix this by balancing mass and thrust instead
    vec3 gravity = vec3(0, -95.0f, 0);
    vec3 netForce = thrust + (gravity * mass);
    acceleration = netForce / mass;

    // calculate the position through acceleration & velocity
    velocity += acceleration * dt;
    velocity *= .99f;
    position += velocity * dt;

    if (length(velocity) > maxVelocity) {
      velocity = normalize(velocity) * maxVelocity;
    }
  }

  void updateTrickState(float dt) {
    // we need to calculate the delta angles for pitch, yaw, and
    // roll to see if we complete full rotations
    glm::quat deltaQ = glm::inverse(prevorientation) * orientation;
    glm::vec3 eulerDelta = glm::eulerAngles(deltaQ);
    // these hold the delta values in case we are able to add it to total
    // accum
    dRoll = glm::degrees(eulerDelta.z);
    dPitch = glm::degrees(eulerDelta.x);
    dYaw = glm::degrees(eulerDelta.y);
    trick = join(trickManager.trickArray, " + ");
    score = trickManager.score;
    vec3 up = orientation * vec3(0, 1, 0);
    trickManager.update(dPitch, dRoll, dYaw, up, dt, maxTricktime);
    prevorientation = orientation;
  }

  // we need this to be able to update the drones orientation based
  // off the inputs from the controller
  void updateOrientation(float rollVel, float pitchVel, float yawVel,
                         float deltaTime) {
    // Create quaternions around local axes (apply roll -> pitch ->  yaw)
    float rollDelta = rollVel * deltaTime;
    float pitchDelta = pitchVel * deltaTime;
    float yawDelta = yawVel * deltaTime;

    glm::quat qRoll = glm::angleAxis(rollDelta, glm::vec3(0, 0, 1)); // local Z
    glm::quat qPitch =
        glm::angleAxis(pitchDelta, glm::vec3(1, 0, 0));            // local X
    glm::quat qYaw = glm::angleAxis(yawDelta, glm::vec3(0, 1, 0)); // local Y
    orientation = orientation * qYaw * qPitch * qRoll;
    orientation = glm::normalize(orientation);
  }

  void updateMouseOrientation(float phi, float theta, float dt) {
    glm::quat qPitch =
        glm::angleAxis(glm::radians(phi), glm::vec3(1, 0, 0)); // pitch around X
    glm::quat qYaw =
        glm::angleAxis(glm::radians(theta), glm::vec3(0, 1, 0)); // yaw around Y

    orientation = qYaw * qPitch;

    orientation = glm::normalize(orientation);
  }
};

#endif
