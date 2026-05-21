// TrickSystem.h
// Used for wiring everything together and holds the drone
// Basic Pipeline: Extract Motion -> Convert To Numerical Features -> Detect Any
// Meaningful Motion Events and Emit -> Match Event to Trick

#include "../Drone.h"
#include "Motion/DroneFeatureExtractor.h"
#include "Motion/DroneMotionBuffer.h"
#include "Motion/MotionEventBuffer.h"
#include "Motion/MotionEventDetector.h"

class TrickSystem {
public:
  void Update(float dt, const Drone &drone);

private:
  DroneMotionBuffer motionBuffer; // used for holding the low level motion
  DroneFeatureExtractor featureExtractor;
  MotionEventDetector
      eventDetector; // used for operating on the features extracted
  MotionEventBuffer
      eventBuffer; // Used for holding the extracted events by eventDetector

  // TrickRegistry trickRegistry;
  // TrickResolver trickResolver;
  // ComboTracker comboTracker;
  // TrickAwarder awarder;
};
