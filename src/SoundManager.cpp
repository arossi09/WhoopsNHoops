#include "SoundManager.h"
#include "SoundBuffer.h"

// this function is needed to initlize sound
// device and contexg as well as load each
// sound file into the buffer
bool SoundManager::init() {
  // p_SoundDevice = SoundDevice::get();
  p_CrashSound = SoundBuffer::get()->addSoundEffect(
      (p_ResourceDir + "/sounds/crash.wav").c_str());
  p_BatteryPickupSound = SoundBuffer::get()->addSoundEffect(
      (p_ResourceDir + "/sounds/entity_pickup.wav").c_str());
  p_DroneSound = SoundBuffer::get()->addSoundEffect(
      (p_ResourceDir + "/sounds/drone16.wav").c_str());
  p_DroneArmSound = SoundBuffer::get()->addSoundEffect(
      (p_ResourceDir + "/sounds/arm.wav").c_str());
  p_DroneDisarmSound = SoundBuffer::get()->addSoundEffect(
      (p_ResourceDir + "/sounds/disarm.wav").c_str());
  p_SpecialSound = SoundBuffer::get()->addSoundEffect(
      (p_ResourceDir + "/sounds/special.wav").c_str());
  p_SpecialSoundSource.setGain(0.1f);
  p_DroneSoundSource.loopSound();
  p_DroneSoundSource.setGain(1.5f);
  // TODO set up other sounds
  return 1;
}

void SoundManager::changeSoundPitch(SoundEffect sound, float pitch) {
  switch (sound) {
  case DRONE_PROPELLER:
    p_DroneSoundSource.setPitch(pitch);
    break;
  case CRASH:
    break;
  case BATTERY_PICKUP:
    break;
  case DRONE_ARM:
    break;
  case DRONE_DISARM:
    break;
  case BONUS_PICKUP:
    break;
  case SPECIAL:
    break;
  }
}
void SoundManager::setResourceDir(const std::string &resourceDir) {
  p_ResourceDir = resourceDir;
}

// this function is needed to take a sound effect
// enum defined in soundmanager.h and play the correlating
// sound from the sound buffer
void SoundManager::play(SoundEffect sound) {
  switch (sound) {
  case DRONE_PROPELLER:
    p_DroneSoundSource.play(p_DroneSound);
    break;
  case CRASH:
    p_SoundSource1.play(p_CrashSound);
    break;
  case BATTERY_PICKUP:
    p_SoundSource2.play(p_BatteryPickupSound);
    break;
  case BONUS_PICKUP:
    break;
  case DRONE_ARM:
    p_SoundSource1.play(p_DroneArmSound);
    break;
  case DRONE_DISARM:
    p_SoundSource1.play(p_DroneDisarmSound);
    break;
  case SPECIAL:
    p_SpecialSoundSource.play(p_SpecialSound);
    break;
  }
}

void SoundManager::stop(SoundEffect sound) {
  switch (sound) {
  case DRONE_PROPELLER:
    p_DroneSoundSource.stop(p_DroneSound);
    break;
  case CRASH:
    break;
  case BATTERY_PICKUP:
    break;
  case DRONE_ARM:
    break;
  case DRONE_DISARM:
    break;
  case BONUS_PICKUP:
    break;
  case SPECIAL:
    break;
  }
}
