#ifndef TRICKSM_H
#define TRICKSM_H

// this class is in charge of bundling each tricks
// state machine together and handling their results

#include <glm/glm.hpp>

enum class TrickType { Roll, SplitS, Flip };
enum class TrickStage { NONE, TIMERSTART, INVERTED, COMPLETE, COOLDOWN };

class TrickStateMachine {
public:
  TrickType type;
  TrickStage state = TrickStage::NONE;
  float acum = 0.0f;
  float timer = 0.0f;

  float pitchAcum = 0.f;
	float pitchIVWAcum = 0.;
  float rollAcum = 0.f;
  float flipAcum = 0.f;
  float rollTriggerTimer = 0.f;
  float rollTriggerAcum = 0.f;
  float splitSTimer = 0.f;
  float rollTimer = 0.f;
  float flipTimer = 0.f;

	//src/TrickStateMachine
  TrickStateMachine(TrickType type) : type(type) {}
  bool update(float dPitch, float dRoll, float dYaw, const glm::vec3 &up,
              float dt, float maxTrickTime) {
    timer += dt;

    switch (type) {
    case TrickType::Roll:
      return handleRoll(dRoll, up, dt, maxTrickTime);
      break;
    case TrickType::SplitS:
      return handleSplitS(dPitch, dRoll, up, dt, maxTrickTime);
			break;
    case TrickType::Flip:
      return handleFlip(dPitch, up, dt, maxTrickTime);
      break;
    }
  }

private:
  bool handleSplitS(float dPitch, float dRoll, const glm::vec3 &up, float dt,
                    float maxTrickTime);

  bool handleFlip(float dPitch, const glm::vec3 &up, float dt,
                  float maxTrickTime);

  bool handleRoll(float dRoll, const glm::vec3 &up, float dt,
                  float maxTrickTime);
};

#endif
