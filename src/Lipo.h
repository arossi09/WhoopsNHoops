#pragma once
#include "AABB.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Drone.h"
#include "Entity.h"
#include "Texture.h"
#include "Shape.h"

class Lipo : public Entity {

public:
  std::shared_ptr<AABB> lipo_AABB;
  std::shared_ptr<Shape> shape;
	glm::vec3 position;
  bool render = true;

  Lipo(glm::vec3 pos, const std::string resourceDirectory);
	void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model) override;
  void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model,
            std::shared_ptr<MatrixStack> View,
            std::shared_ptr<MatrixStack> Project);
  void update(float dt, Drone &drone) override;
  std::shared_ptr<AABB> getAABB() override;
  // need to add function to call once i detect collision that takes
  // and alters drone state
	//  
private:
	std::vector<glm::vec3> possible_locations = {glm::vec3(-21, 6, 3), glm::vec3(-9, 0, -16),
		glm::vec3(0, 4, -4), glm::vec3(1, 2, 5) };
  //std::vector<vec3> = {vec3(3, 4, 5)};
  std::shared_ptr<Program> shadowProg;
	std::shared_ptr<Texture> lipo_texture;
  void chargeBattery(Drone drone);
	//we need this to generate a new random position for lipo after collected
	void newRandPosition();
};
