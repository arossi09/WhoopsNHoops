// MotionEventBuffer.h
// Used for holding the motion events
// in a ring buffer which is held
// by the TrickSystem bridging motion to Recognizers
#include "MotionEvent.h"
#include "RingBuffer.h"

class MotionEventBuffer {
public:
  void Push(const MotionEvent &event);
  void RemoveOlderThan(double time);

  std::span<const MotionEvent> Recent() const;

private:
  RingBuffer<MotionEvent 256> events;
};
