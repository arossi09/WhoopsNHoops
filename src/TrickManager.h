#include "TrickSM.h"
#include "iostream"

// this class is in charge of creating each
// state machine and handling their outputs
class TrickManager {
public:
  std::vector<std::string> trickArray;
  float score = 0.0f;
  float timeSinceLastTrick = 0.f;
	int trickCount = 0;
	int scoreMultipler = 1;

  TrickStateMachine splitSFSM{TrickType::SplitS};
  TrickStateMachine rollFSM{TrickType::Roll};
  TrickStateMachine flipFSM{TrickType::Flip};

	void incrementMultipler(){
		scoreMultipler++;
		score *= scoreMultipler;
	}

	//src/TrickManager
  void update(float dPitch, float dRoll, float dYaw, const glm::vec3 &up,
              float dt, float maxTrickTime) {

    timeSinceLastTrick += dt;
		if(trickArray.size() > 10)
			trickArray.clear();

    if (splitSFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)) {
      trickArray.push_back("splitS");
      score += 500;
			trickCount++;
      resetTrickTimer();
			return;
    }

    if (rollFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)) {
      trickArray.push_back("roll");
      score += 150;
			trickCount++;
      resetTrickTimer();
			return;
    }

		if(flipFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)){
      trickArray.push_back("flip");
      score += 150;
			trickCount++;
      resetTrickTimer();
			return;
		}

    if (timeSinceLastTrick > 8.0f) {
      resetAll();
    }
  }

	void reset(){
		resetAll();
	}

private:
  void resetTrickTimer() { timeSinceLastTrick = 0.0f; }
  void resetAll() {
    timeSinceLastTrick = 0.0f;
    trickArray.clear();
    score = 0.0f;
		trickCount = 0;
		//scoreMultipler= 1;
  }
};
