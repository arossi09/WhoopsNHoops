// DroneFeatureExtractor.h
// Second step in trick system motion extraction
// used for extracting the motion measurments into numerical meanings
#include "DroneMotionBuffer.h"

/* Holds Numerical meaning extracted
 * from drone motions
 */
struct DroneFeatureFrame {
  double time = 0.0f;

  float rollRate = 0.0f;
  float pitchRate = 0.0f;
  float yawRate = 0.0f;

  float recentPitchDegrees = 0.0f;
  float recentRollDegrees = 0.0f;
  float recentYawDegrees = 0.0f;

  bool isInverted = false;
  float invertedAmount = 0.0f;

  float forwardSpeed = 0.0f;
  float verticalSpeed = 0.0f;

  float diveAngle = 0.0f;
  bool isRecovering = false;
};

class DroneFeatureExtractor {
public:
  DroneFeatureFrame Extract(const DroneMotionBuffer &motionBuffer);
};
