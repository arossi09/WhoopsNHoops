#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H
#include "SoundDevice.h"
#include "SoundSource.h"
#include <OpenAL/al.h>
#include <iostream>

typedef enum {
  DRONE_PROPELLER,
  CRASH,
  BATTERY_PICKUP,
  BONUS_PICKUP,
} SoundEffect;

class SoundManager {
public:
  SoundManager() : p_SoundDevice(SoundDevice::get()), p_SoundSource() {};
  void setResourceDir(const std::string &resourceDir);
  bool init();
  void play(SoundEffect sound);

private:
  std::string p_ResourceDir;
  SoundDevice *p_SoundDevice;
  SoundSource p_SoundSource;
  ALuint p_DroneSound = 0;
  ALuint p_CrashSound = 0;
  ALuint p_BatteryPickupSound = 0;
  ALuint p_BonusPickupSound = 0;
};
#endif
