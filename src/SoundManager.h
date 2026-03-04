#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H
#include "SoundDevice.h"
#include "SoundSource.h"
#include <OpenAL/al.h>
#include <iostream>

typedef enum {
  DRONE_PROPELLER,
	DRONE_ARM,
	DRONE_DISARM,
  CRASH,
  BATTERY_PICKUP,
  BONUS_PICKUP
} SoundEffect;

class SoundManager {
public:
  SoundManager()
      : p_SoundDevice(SoundDevice::get()), p_SoundSource1(),
        p_SoundSource2(), p_DroneSoundSource() {};
  void setResourceDir(const std::string &resourceDir);
  bool init();
  void play(SoundEffect sound);
	void stop(SoundEffect sound);
	void changeSoundPitch(SoundEffect sound, float pitch);

private:
  std::string p_ResourceDir;
  SoundDevice *p_SoundDevice;
  SoundSource p_SoundSource1;
  SoundSource p_SoundSource2;
	SoundSource p_DroneSoundSource;
  ALuint p_DroneSound = 0;
  ALuint p_CrashSound = 0;
  ALuint p_BatteryPickupSound = 0;
  ALuint p_BonusPickupSound = 0;
	ALuint p_DroneArmSound = 0;
	ALuint p_DroneDisarmSound= 0;
};
#endif
