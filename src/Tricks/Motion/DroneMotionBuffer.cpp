#include "DroneMotionBuffer.h"

void DroneMotionBuffer::Push(const DroneMotionSample &sample) {
  samples.push(sample)
}

void DroneMotionBuffer::Clear() { samples.clear() }
