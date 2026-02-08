#pragma once
#include "AABB.h"
#include "Drone.h"
#include "MatrixStack.h"
#include "Program.h"

class Entity {
public:
  virtual ~Entity() {};
  virtual void update(float dt, Drone &drone) = 0;
  virtual std::shared_ptr<AABB> getAABB() = 0;
  virtual void draw(std::shared_ptr<Program> prog,
                    std::shared_ptr<MatrixStack> Model, Drone &drone) = 0;

  // getters and setters
  virtual glm::vec3 getPos() { return position; }
  virtual void setPos(glm::vec3 new_pos) { position = new_pos; }
  virtual bool getNeedRespawn() { return needsRespawn; }
  virtual void setNeedRespawn(bool v) { needsRespawn = v; }

protected:
  glm::vec3 position{};
  bool needsRespawn = false;
};
