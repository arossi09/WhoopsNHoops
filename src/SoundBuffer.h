#ifndef SOUNDBUFFER_H
#define SOUNDBUFFER_H
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>

class SoundBuffer {
public:
  static SoundBuffer *get();

  ALuint addSoundEffect(const char *filename);
  bool removeSoundEffect(const ALuint &buffer);

private:
  SoundBuffer();
  ~SoundBuffer();

  std::vector<ALuint> p_SoundEffectBuffers;
};
#endif
