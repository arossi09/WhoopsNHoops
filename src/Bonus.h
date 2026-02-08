#ifndef BONUS_H
#define BONUS_H
#include "Entity.h"
#include "AABB.h"
#include "Drone.h"
#include "Shape.h"
#include "Texture.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

class Bonus : public Entity {
public:
  std::shared_ptr<AABB> bonus_AABB;
  std::shared_ptr<Shape> shape;

  Bonus(const std::string &resourceDirectory);
  void draw(std::shared_ptr<Program> prog,
            std::shared_ptr<MatrixStack> Model, Drone &drone) override;
  void update(float dt, Drone &drone) override;
  std::shared_ptr<AABB> getAABB() override;

private:
  std::shared_ptr<Texture> bonus_texture;
  // we need this to generate a new random position for lipo after collected
  void newRandPosition();

};
#endif
