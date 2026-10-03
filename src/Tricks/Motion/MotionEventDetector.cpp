#include "MotionEventDetector.h"
#include "MotionEvent.h"

/* Produces a Motion Event based off the
 * stream of features
 */
void MotionEventDetector::Update(const DroneFeatureFrame &features,
                                 MotionEventBuffer &outEvents) {
  if (!hasPreviousFrame) {
    previousTime = features.time;
    hasPreviousFrame = true;
    return;
  }

  const float dt = features.time - previousTime;
  previousTime = features.time;

  //Handle inversion events
  if (!wasInverted && features.isInverted) {
    MotionEvent e;
    e.type = MotionEventType::InvertedStarted;
    e.startTime = features.time;
    e.endTime = features.time;
    outEvents.Push(e);
    printf("Invert Started\n");
  }

  if(wasInverted && !features.isInverted){
    MotionEvent e;
    e.type = MotionEventType::InvertedEnded;
    e.startTime = features.time;
    e.endTime = features.time;
    outEvents.Push(e);
    printf("Invert Ended\n");
  }
  wasInverted = features.isInverted;

  //TODO roll, pitch, yaw events
}
