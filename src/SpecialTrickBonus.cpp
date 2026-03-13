#include "SpecialTrickBonus.h"
#include "Drone.h"
#include "Program.h"
#include <cstdlib>
#include <iostream>

SpecialTrickBonus::SpecialTrickBonus(const std::string &resourceDirectory) {
  // set defualt position
  needsRespawn = true;
  // initilize the bonus texture for loading later
  trickbonus_texture = std::make_shared<Texture>();
  trickbonus_texture->setFilename(resourceDirectory + "/specialtrick.png");
  trickbonus_texture->init();
  trickbonus_texture->setUnit(0);
  trickbonus_texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  trickbonus_texture->setFiltering(GL_NEAREST, GL_NEAREST);
  // create shape
  std::vector<tinyobj::shape_t> TOshapes;
  std::vector<tinyobj::material_t> objMaterials;
  // load in the mesh and make the shape(s)
  std::string errStr;
  bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                             (resourceDirectory + "/specialtrick.obj").c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {
    shape = std::make_shared<Shape>();
    shape->createShape(TOshapes[0]);
    shape->measure();
    shape->init();
  }
  // create AABB
  trickbonus_AABB = std::make_shared<AABB>(shape->min, shape->max);
}

// we need this to draw and transform the AABB
void SpecialTrickBonus::draw(std::shared_ptr<Program> prog,
                             std::shared_ptr<MatrixStack> Model, Drone &drone) {
  if (drone.special_mode) {
    trickbonus_texture->bind(prog->getUniform("Texture0"));
    Model->pushMatrix();
    Model->translate(glm::vec3(position.x, position.y, position.z));
    Model->rotate(glfwGetTime(), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                       value_ptr(Model->topMatrix()));
    trickbonus_AABB->transform(Model->topMatrix());
    shape->draw(prog);
    Model->popMatrix();
  }
}

void SpecialTrickBonus::update(float dt, Drone &drone) {
  if (drone.special_mode) {
    drone.scoreSpecialTrickBonus();
    needsRespawn = true;
  }
}

std::shared_ptr<AABB> SpecialTrickBonus::getAABB() { return trickbonus_AABB; }
