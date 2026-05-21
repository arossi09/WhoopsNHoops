// MotionEventDetector
// Used for operating on DroneFeatureFrames
// to detect motion which is sent to the
// MotionEventBuffer held by the TrickSystem

#include "DroneFeatureExtractor.h"
#include "MotionEventBuffer.h"

class MotionEventDetector {
public:
  void Update(const DroneFeatureFrame & features, MotioneventBuffer &outEvents);

private:
  bool wasInverted = false;

  bool rollActive = false;
  double rollStartTime = 0.0f;
  float accumulatedRollDegrees = 0.0f;

  double previousTime = 0.0f;
  bool hasPreviousFrame = false;
};
