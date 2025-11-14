#include "TrickSM.h"
#include "iostream"

// this class is in charge of creating each
// state machine and handling their outputs
class TrickManager {
public:
  std::vector<std::string> trickArray;
  float score = 0.0f;
  float timeSinceLastTrick = 0.f;

  TrickStateMachine splitSFSM{TrickType::SplitS};
  TrickStateMachine rollFSM{TrickType::Roll};
  TrickStateMachine flipFSM{TrickType::Flip};

	//src/TrickManager
  void update(float dPitch, float dRoll, float dYaw, const glm::vec3 &up,
              float dt, float maxTrickTime) {

    timeSinceLastTrick += dt;
		if(trickArray.size() > 10)
			trickArray.clear();

    if (splitSFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)) {
      trickArray.push_back("splitS");
      score += 500;
      resetTrickTimer();
			return;
    }

    if (rollFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)) {
      trickArray.push_back("roll");
      score += 150;
      resetTrickTimer();
			return;
    }

		if(flipFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)){
      trickArray.push_back("flip");
      score += 150;
      resetTrickTimer();
			return;
		}

    if (timeSinceLastTrick > 8.0f) {
      resetAll();
    }
  }

private:
  void resetTrickTimer() { timeSinceLastTrick = 0.0f; }
  void resetAll() {
    timeSinceLastTrick = 0.0f;
    trickArray.clear();
    score = 0.0f;
  }
};
