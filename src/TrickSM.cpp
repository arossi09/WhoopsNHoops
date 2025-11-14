#include "TrickSM.h"
#include "iostream"

// This function handles the different states of the drone
// which activate the splitS trick detection
bool TrickStateMachine::handleSplitS(float dPitch, float dRoll,
                                     const glm::vec3 &up, float dt,
                                     float maxTrickTime) {
  switch (state) {
  case TrickStage::NONE:
    pitchAcum = 0;
    splitSTimer = 0;
    if (abs(dRoll) > 0.5) {
      rollTriggerTimer += dt;
      rollTriggerAcum += dRoll;

      if (abs(rollTriggerAcum) >= 150.f && rollTriggerTimer <= maxTrickTime) {
        state = TrickStage::INVERTED;
        splitSTimer = 0;
        pitchAcum = 0;
        rollTriggerAcum = 0;
        rollTriggerTimer = 0;
      } else if (rollTriggerTimer > maxTrickTime) {
        rollTriggerAcum = 0;
        rollTriggerTimer = 0;
      }
    }
    // std::cout << "NONE" << std::endl;
    break;
  case TrickStage::INVERTED:
    if (up.y > 0)
      state = TrickStage::NONE;
    // std::cout << "UPSIDE DOWN" << std::endl;
    splitSTimer += dt;
    pitchAcum += dPitch;

    if (splitSTimer > maxTrickTime) {
      pitchAcum = 0;
      state = TrickStage::NONE;
    } else if (pitchAcum >= 45.0f) {
      pitchAcum = 0;
      return true;
      state = TrickStage::COMPLETE;
    }
    break;
  case TrickStage::COMPLETE:
    state = TrickStage::COOLDOWN;
		splitSTimer = 0;
		pitchAcum = 0;
    break;
	case TrickStage::COOLDOWN:
    splitSTimer += dt;
    if (splitSTimer >= 1.0f) {
      splitSTimer = 0;
      state = TrickStage::NONE;
    }

  case TrickStage::TIMERSTART:
    break;
  }
  return false;
}

bool TrickStateMachine::handleRoll(float dRoll, const glm::vec3 &up, float dt,
                                   float maxTrickTime) {
  switch (state) {
  case TrickStage::NONE:

    if (abs(dRoll) > 5.0) {
      state = TrickStage::TIMERSTART;
      rollTimer = 0;
      rollAcum = 0;
    }
    break;
  case TrickStage::TIMERSTART:
    rollTimer += dt;
    rollAcum += dRoll;
    if (abs(rollAcum) >= 180.0f && rollTimer <= maxTrickTime) {
      state = TrickStage::INVERTED;
    } else if (rollTimer >= maxTrickTime) {
      state = TrickStage::NONE;
    }
    break;
  case TrickStage::INVERTED:
    if (up.y > -.4)
      state = TrickStage::NONE;
    rollTimer += dt;
    rollAcum += dRoll;
    if (rollTimer > maxTrickTime) {
      rollAcum = 0;
      state = TrickStage::NONE;
    } else if (abs(rollAcum) >= 180.0f) {
      state = TrickStage::COMPLETE;
    }
    break;
  case TrickStage::COMPLETE:
    state = TrickStage::COOLDOWN;
    rollTimer = 0;
    rollAcum = 0;
    return true;
    break;

  case TrickStage::COOLDOWN:
    rollTimer += dt;
    if (rollTimer >= 1.0f) {
      rollTimer = 0;
      state = TrickStage::NONE;
    }
  }
  return false;
}

bool TrickStateMachine::handleFlip(float dPitch, const glm::vec3 &up, float dt,
                                   float maxTrickTime) {
  switch (state) {
  case TrickStage::NONE:
    // if threshold for dPitch start timer
    // state
    if (abs(dPitch) > 0.5f) {
      state = TrickStage::TIMERSTART;
      flipTimer = 0;
      flipAcum = 0;
    }
    break;
  case TrickStage::TIMERSTART:
    flipTimer += dt;
    flipAcum += dPitch;
    if (abs(flipAcum) >= 180.0f && flipTimer <= maxTrickTime) {
      state = TrickStage::INVERTED;
    } else if (flipTimer >= maxTrickTime) {
      state = TrickStage::NONE;
    }
    break;
  case TrickStage::INVERTED:
    if (up.y > -.4)
      state = TrickStage::NONE;
    flipTimer += dt;
    flipAcum += dPitch;
    if (flipTimer > maxTrickTime) {
      flipAcum = 0;
      state = TrickStage::NONE;
    } else if (abs(flipAcum) >= 180.0f) {
      state = TrickStage::COMPLETE;
    }
		break;
  case TrickStage::COMPLETE:
    state = TrickStage::COOLDOWN;
    flipTimer = 0;
    flipAcum = 0;
    return true;
    break;
  case TrickStage::COOLDOWN:
    flipTimer += dt;
    if (flipTimer >= 1.0f) {
      state = TrickStage::NONE;
    }
  }
}

/*
bool TrickStateMachine::handleIVW(float dPitch, const glm::vec3 &up, float dt, float maxTrickTime){

	switch(state){
		case TrickStage::NONE:

			if(abs(dPitch) > 0.5f){
				state = TrickStage::TIMERSTART;
				pitchIVWAcum = 0;
			}
	}
}
*/
