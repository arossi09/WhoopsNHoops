#include "SoundManager.h"
#include "SoundBuffer.h"

// this function is needed to initlize sound
// device and contexg as well as load each
// sound file into the buffer
bool SoundManager::init() {
  //p_SoundDevice = SoundDevice::get();
  p_CrashSound = SoundBuffer::get()->addSoundEffect(
      (p_ResourceDir + "/sounds/crash.wav").c_str());
  // TODO set up other sounds
  return 1;
}

void SoundManager::setResourceDir(const std::string &resourceDir) {
  p_ResourceDir = resourceDir;
}

// this function is needed to take a sound effect
// enum defined in soundmanager.h and play the correlating
// sound from the sound buffer
void SoundManager::play(SoundEffect sound) {
  // TODO handle other cases sounds
  switch (sound) {
  case DRONE_PROPELLER:
    break;
  case CRASH:
		printf("Playing Crash Sound\n");
    p_SoundSource.play(p_CrashSound);
    break;
  case BATTERY_PICKUP:
    break;
  case BONUS_PICKUP:
    break;
  }
}
