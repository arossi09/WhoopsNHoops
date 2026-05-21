// DroneMotionBuffer.h
// start of the trick system
// Holds the low level raw drone data in a ring buffer
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "RingBuffer.h"
#include <glm/gtx/quaternion.hpp>
#include <iostream>

struct DroneMotionSample {
  double time = 0.0f;

  glm::vec3 position;
  glm::quat rotation;

  glm::vec3 velocityWorld;
  glm::vec3 angularVelocityWorld;

  glm::vec3 velocityLocal;
  glm::vec3 angularVelocityLocal;

  float throttle = 0.0f;
  float pitchInput = 0.0f;
  float rollInput = 0.0f;
  float yawInput = 0.0f;

  float altitude = 0.0f;
  bool nearGround = false;
};

class DroneMotionBuffer {
public:
  void Push(const DroneMotionSample &sample);
  void Clear();

  const DroneMotionSample *Latest();

  std::span<const DroneMotionSample> Recent() const;
  std::span<const DroneMotionSample> Since(double time) const;

private:
  RingBuffer<DroneMotionSample, 300> samples;
}
