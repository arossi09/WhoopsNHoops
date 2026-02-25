#include "SoundSource.h"

// we need to generate the source and
// initlize the given source paramters such as
// pitch, position, velocity, looping, and buffer
SoundSource::SoundSource() {
  alGenSources(1, &p_Source);
  alSourcef(p_Source, AL_PITCH, p_Pitch);
  alSource3f(p_Source, AL_POSITION, p_Position[0], p_Position[1],
             p_Position[2]);
  alSource3f(p_Source, AL_VELOCITY, p_Velocity[0], p_Velocity[1],
             p_Velocity[2]);
  alSourcei(p_Source, AL_LOOPING, p_LoopSound);
  alSourcei(p_Source, AL_BUFFER, p_Buffer);
}

SoundSource::~SoundSource() { alDeleteSources(1, &p_Source); }

void SoundSource::play(const ALuint buffer_to_play) {
  if (buffer_to_play != p_Buffer) {
    p_Buffer = buffer_to_play;
    alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
  }

  alSourcePlay(p_Source);
}
