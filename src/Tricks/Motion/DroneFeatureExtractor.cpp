#include "DroneFeatureExtractor.h"

//Used for extracting the angular velocity from a certain axis of rotation
float GetAngularRateForAxis(glm::vec3 angularVelocity, glm::vec3 axis) {
  return glm::dot(angularVelocity, axis);
}

//used for calculating the angular rotation of a given
//axis over a set of motion samples
float CalculateRecentAngleDelta(std::span<const DroneMotionSample> samples,
                                glm::vec3 axis) {
  if (samples.size() < 2)
    return 0.0f;

  float totalRadians = 0.0f;

  for (size_t i = 1; i < samples.size(); ++i) {
    const DroneMotionSample &previous = samples[i - 1];
    const DroneMotionSample &current = samples[i];

    const double dt = current.time - previous.time;
    const float axisRate =
        GetAngularRateForAxis(current.angularVelocityLocal, axis);

    // radians/sec * sec = radians for that sample
    totalRadians += axisRate * static_cast<float>(dt);
  }

  return glm::degrees(totalRadians)
}

DroneFeatureFrame
DroneFeatureExtractor::Extract(const DroneMotionBuffer &motionBuffer) {
  DroneFeatureFrame features;

  // grab the latest motion sample and operate on it
  const DroneMotionSample *latest = motionBuffer.Latest();
  if (!latest)
    return features;

  features.time = latest->time;

  features.rollRate = latest->angularVelocityLocal.z;
  features.pitchRate = latest->angularVelocityLocal.x;
  features.yawRate = latest->angularVelocityLocal.y;

  features.forwardSpeed = latest->velocityLocal.z;
  features.verticalSpeed = latest->velocityWorld.y;

  glm::vec3 up = latest->rotation * glm::vec3(0, 1, 0);
  float upDot = dot(up, glm::vec3(0, 1, 0));

  features.invertedAmount = -upDot;
  features.isInverted = upDot < -0.65f;

  auto recentSamples = motionBuffer.Since(latest->time - 0.5);
  features.recentRollDegrees =
      CalculateRecentAngleDelta(recentSamples, glm::vec3(0, 0, 1));
  features.recentYawDegrees =
      CalculateRecentAngleDelta(recentSamples, glm::vec3(0, 1, 0));
  features.recentPitchDegrees =
      CalculateRecentAngleDelta(recentSamples, glm::vec3(1, 0, 0));

  return features;
}
