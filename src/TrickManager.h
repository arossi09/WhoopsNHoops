#include "SoundManager.h"
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

  void incrementMultipler() {
    scoreMultipler++;
    score *= scoreMultipler;
  }

  void addBonus() {
    score += 50.0f;
    trickArray.push_back("BONUS");
  }

  void addSpecialTrickBonus() {
    score += 250.0f;
    trickCount++;
    trickArray.push_back("!");
  }

  // src/TrickManager
  void update(float dPitch, float dRoll, float dYaw, const glm::vec3 &up,
              float dt, float maxTrickTime, float *styleScore,
              SoundManager &sm) {

    timeSinceLastTrick += dt;
    if (trickArray.size() > 10)
      trickArray.erase(trickArray.begin());

    if (splitSFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)) {
      trickArray.push_back("splitS");
      score += 200;
      *styleScore += 500;
      trickCount++;
			//sm.play(TRICK);
      resetTrickTimer();
      return;
    }

    if (rollFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)) {
      trickArray.push_back("roll");
      score += 100;
      *styleScore += 150;
      trickCount++;
			//sm.play(TRICK);
      resetTrickTimer();
      return;
    }

    if (flipFSM.update(dPitch, dRoll, dYaw, up, dt, maxTrickTime)) {
      trickArray.push_back("flip");
      score += 100;
      *styleScore += 150;
      trickCount++;
			//sm.play(TRICK);
      resetTrickTimer();
      return;
    }
  }

  void reset() { resetAll(); }

private:
  void resetTrickTimer() { timeSinceLastTrick = 0.0f; }
  void resetAll() {
    timeSinceLastTrick = 0.0f;
    trickArray.clear();
    score = 0.0f;
    trickCount = 0;
    // scoreMultipler= 1;
  }
};
