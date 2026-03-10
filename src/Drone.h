#ifndef DRONE_H
#define DRONE_H

#include "AABB.h"
#include "TrickManager.h"
#include <glm/glm.hpp>
#include <iostream>
#include <string>

using namespace glm;

#define BASE_DECAY_RATE .01
#define THROTTLE_FACTOR .8

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

// drone struct with attributes and update
struct Drone {
  glm::vec3 light_blue = {0.56, 0.9, 1.0};
  glm::vec3 gold = {1.0, 0.9, 0.0};
  float battery = 100.0f;	
	float batteryDecayRate = BASE_DECAY_RATE;
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
  bool armed = false;

  glm::vec3 droneColor = light_blue;

  std::string trick = "";
  std::string oldTrick = "";
  float styleScore = 0.0f;
  float scoreDecayRate = 15.0;
  int score = 0;
  int oldScore = 0;
  int oldTrickCount = 0;
  int totalScore = 0;
  bool special_mode = false;
  float special_score_thresh = 1500.0f;
  int trickCount = 0;
  // final stats
  int obstaclesHit = 0;
  int finalScore = 0;
  int batteriesCollected = 0;
  int totalCombos = 0;
  int highestCombo = 0;

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
  bool touching_collider = false;
  bool was_touching = false;

  AABB getAABB() const {
    float halfSize = .7f;
    return AABB(position - glm::vec3(halfSize), position + glm::vec3(halfSize));
  }

  bool getWasTouching() { return was_touching; }

  void setWasTouching(bool val) { was_touching = val; }
  bool getTouchingCollider() { return touching_collider; }

  void setTouchingCollider(bool val) { touching_collider = val; }

  int getObstaclesHit() { return obstaclesHit; }

  void setObstaclesHit(int num) { obstaclesHit = num; }

  int getBatteriesCollected() { return batteriesCollected; }

  void setBatteriesCollected(int num) { batteriesCollected = num; }

  void getPosition() {
    std::cout << "Drone Position: " << "x: " << position.x
              << " y: " << position.y << " z: " << position.z << '\n';
  }

	void increaseDifficulty(int difficultyLevel){
		batteryDecayRate *= 1.17;
	}

  void setArmed(bool state) { armed = state; }
  bool getArmed() { return armed; }

  void chargeBattery() { battery = 100.0f; }

  void scoreBonus() { trickManager.addBonus(); }

  // we need this to reset the state of the drone
  // on gameovers
  void reset() {
    armed = false;
    orientation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    obstaclesHit = 0;
    finalScore = 0;
    batteriesCollected = 0;
    totalCombos = 0;
    highestCombo = 0;
    oldScore = 0;
    oldTrick = "";
    oldTrickCount = 0;
    totalScore = 0.0f;
    position = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    trickManager.reset();
  }

  void endCombo() {
    if (score > 0) {
      highestCombo = max(score, highestCombo);
      totalScore += score;
      finalScore = totalScore;
      totalCombos += 1;
      oldScore = score;
      oldTrick = trick;
      oldTrickCount = trickCount;
      score = 0;
      styleScore = 0;
      trickManager.reset();
    }
  }

  // calculate drone physics
  void updatePosition(float dt) {
    if (!armed)
      throttle = 0;
    battery -= batteryDecayRate + THROTTLE_FACTOR * throttle * dt;
    battery = max(battery, 0.0f);
    if (battery > 100.0f) {
      battery = 100.0f;
    }
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
    trickCount = trickManager.trickCount;
    vec3 up = orientation * vec3(0, 1, 0);
    trickManager.update(dPitch, dRoll, dYaw, up, dt, maxTricktime, &styleScore);

    if (styleScore > 0)
      styleScore -= dt * scoreDecayRate;

    // we need to set drone to special mode if above score of 3000
    if (styleScore >= special_score_thresh) {
      special_mode = true;
      droneColor = gold;
      maxVelocity = 150.0f;
    } else {
      special_mode = false;
      droneColor = light_blue;
      maxVelocity = 100.0f;
    }

    prevorientation = orientation;
  }

  // we need this to be able to update the drones orientation based
  // off the inputs from the controller
  void updateOrientation(float rollVel, float pitchVel, float yawVel,
                         float deltaTime) {

    // Create quaternions around local axes (apply roll -> pitch ->  yaw)
    float rollDelta = 0;
    float pitchDelta = 0;
    float yawDelta = 0;
    // if the drone is armed update the axis deltas
    if (armed) {
      yawDelta = yawVel * deltaTime;
      pitchDelta = pitchVel * deltaTime;
      rollDelta = rollVel * deltaTime;
    }

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
