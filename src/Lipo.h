#pragma once
#include "AABB.h"
#include "Drone.h"
#include "Entity.h"
#include "Shape.h"
#include "Texture.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

class Lipo : public Entity {

public:
  std::shared_ptr<AABB> lipo_AABB;
  std::shared_ptr<Shape> shape;
  //glm::vec3 position;
 	bool render = true;
	//bool needsRespawn = false;
  Lipo(const std::string &resourceDirectory);
  void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model,
            Drone &drone) override;
  void update(float dt, Drone &drone) override;
  std::shared_ptr<AABB> getAABB() override;
  // need to add function to call once i detect collision that takes
  // and alters drone state

private:
  std::shared_ptr<Program> shadowProg;
  std::shared_ptr<Texture> lipo_texture;
  void chargeBattery(Drone drone);
  // we need this to generate a new random position for lipo after collected
};
