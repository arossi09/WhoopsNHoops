#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H
#include <OpenAL/al.h>
class SoundSource {
public:
  SoundSource();
  ~SoundSource();
  void play(const ALuint buffer_to_play);
	void stop(const ALuint buffer_to_stop);
	void loopSound();
	void setPitch(float new_pitch);
	float getPitch();

private:
  ALuint p_Source;
  float p_Pitch = 1.f;
  float p_Gain = 1.f;
  float p_Position[3] = {0, 0, 0};
  float p_Velocity[3] = {0, 0, 0};
  bool p_LoopSound = false;
  ALuint p_Buffer = 0;
};

#endif
