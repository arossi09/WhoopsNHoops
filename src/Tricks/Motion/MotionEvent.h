// MotionEvent.h
// Structure for holding a motion event which will be matched
// by the trick recognizer

enum class MotionEventType {
  RollStarted,
  RollCompleted,
  PitchFlipStarted,
  PitchFlipCompleted,
  YawSpinCompleted,
  InvertedStarted,
  InvertedEnded,
  DiveStarted,
  DiveEnded,
  RecoveryDetected,
  ThrottleCut,
  ThrottlePunch
};

enum class MotionDirection { None, Left, Right, Forward, Backward, Up, Down };

struct MotionEvent {
  MotionEventType type;
  MotionDirection direction = MotionDirection::None;

  double startTime = 0.0f;
  start endTime = 0.0f;

  float magnitude = 0.0f;
  float confidence = 0.0f;
}
