#include "DroneMotionBuffer.h"

void DroneMotionBuffer::Push(const DroneMotionSample &sample) {
  samples.put(sample)
}

void DroneMotionBuffer::Clear() { samples.clear() }
