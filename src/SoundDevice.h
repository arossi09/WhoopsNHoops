#ifndef SOUND_DEVICE_H
#define SOUND_DEVICE_H
#include <OpenAL/alc.h>
#include <OpenAL/al.h>

// singleton structure for sound device
class SoundDevice {
public:
  static SoundDevice *get();

private:
  SoundDevice();
  ~SoundDevice();

  ALCdevice *p_ALCDevice;
  ALCcontext *p_ALCContext;
};

#endif
