#include "TrickSystem.h"

glm::vec3 ComputeAngularVelocityWorld(const glm::quat &previousOrientation,
                                      const glm::quat &currentOrientation,
                                      float dt) {
  if (dt <= 0.0f)
    return glm::vec3(0.0f);

  // gather rotation delta from quat
  glm::quat delta = currentOrientation * glm::inverse(previousOrientation);
  delta = normalize(delta);

  // make sure shortest rotation path
  if (delta.w < 0.0f)
    delta = -delta;

  float angle = 2.0f * std::acos(glm::clamp(delta.w, -1.0f, 1.0f));
  float sinHalfAngle = std::sqrt(1.0f - delta.w * delta.w);

  glm::vec3 axis;
  if (sinHalfAngle < 0.0001f) {
    axis = glm::vec3(0.0f);
  } else {
    axis = glm::vec3(delta.x, delta.y, delta.z) / sinHalfAngle;
  }
  return axis * (angle / dt); // radians per second
}

void TrickSystem::Update(float dt, const Drone &drone) {
  glm::vec3 angularVelocityWorld =
      ComputeAngularVelocityWorld(drone.prevorientation, drone.orientation, dt);
  glm::vec3 angularVelocityLocal =
      glm::inverse(drone.orientation) * angularVelocityWorld;
  // create sample from drone stats and push to buff
  DroneMotionSample sample;
  sample.time = glfwGetTime();
  sample.position = drone.position;
  sample.rotation = drone.orientation;
  sample.velocityLocal = drone.velocity;
  sample.velocityWorld = drone.velocity;
  sample.angularVelocityLocal = angularVelocityLocal;
  sample.angularVelocityWorld = angularVelocityWorld;
  sample.throttle = drone.throttle;
  sample.pitchInput = drone.pitchInput;
  sample.rollInput = drone.rollInput;
  sample.yawInput = drone.yawInput;
  motionBuffer.Push(sample);

  // extract a meaninful feature from motion buffer
  DroneFeatureFrame feature = featureExtractor.Extract(motionBuffer);

  // detect any event and add it to the event buff
  eventDetector.Update(feature, eventBuffer);

  // TODO match events against recognizers
}
