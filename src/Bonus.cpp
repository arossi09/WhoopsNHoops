#include "Bonus.h"
#include "Drone.h"
#include "Program.h"
#include <cstdlib>
#include <iostream>

Bonus::Bonus(const std::string &resourceDirectory) {
  // set defualt position
	needsRespawn = true;
  // initilize the bonus texture for loading later
  bonus_texture = std::make_shared<Texture>();
  bonus_texture->setFilename(resourceDirectory + "/bonus.png");
  bonus_texture->init();
  bonus_texture->setUnit(0);
  bonus_texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  bonus_texture->setFiltering(GL_NEAREST, GL_NEAREST);
  // create shape
  std::vector<tinyobj::shape_t> TOshapes;
  std::vector<tinyobj::material_t> objMaterials;
  // load in the mesh and make the shape(s)
  std::string errStr;
  bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                             (resourceDirectory + "/bonus.obj").c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {
    shape = std::make_shared<Shape>();
    shape->createShape(TOshapes[0]);
    shape->measure();
    shape->init();
  }
  // create AABB
  bonus_AABB= std::make_shared<AABB>(shape->min, shape->max);

}

// we need this to draw and transform the AABB
void Bonus::draw(std::shared_ptr<Program> prog,
                std::shared_ptr<MatrixStack> Model, Drone &drone) {
		bonus_texture->bind(prog->getUniform("Texture0"));
		Model->pushMatrix();
		Model->translate(
				glm::vec3(position.x, sin(glfwGetTime()) * .3f + position.y, position.z));
		Model->rotate(glfwGetTime()*6, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
				value_ptr(Model->topMatrix()));
		bonus_AABB->transform(Model->topMatrix());
		shape->draw(prog);
		Model->popMatrix();
}


void Bonus::update(float dt, Drone &drone) {
  // charge drone battery;
	if(drone.getHealth()<4)
		drone.setHealth(drone.getHealth()+1);
	needsRespawn = true;
  return;
}


std::shared_ptr<AABB> Bonus::getAABB() { return bonus_AABB; }
