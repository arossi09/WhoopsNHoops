#include "Lipo.h"
#include "Drone.h"
#include "Program.h"
#include <cstdlib>
#include <iostream>

Lipo::Lipo(const std::string &resourceDirectory) {
  // set defualt position
	newRandPosition();
  // initilize the lipo texture for loading later
  lipo_texture = std::make_shared<Texture>();
  lipo_texture->setFilename(resourceDirectory + "/1slipo.png");
  lipo_texture->init();
  lipo_texture->setUnit(0);
  lipo_texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  lipo_texture->setFiltering(GL_NEAREST, GL_NEAREST);
  // create shape
  std::vector<tinyobj::shape_t> TOshapes;
  std::vector<tinyobj::material_t> objMaterials;
  // load in the mesh and make the shape(s)
  std::string errStr;
  bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                             (resourceDirectory + "/1slipo.obj").c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {
    shape = std::make_shared<Shape>();
    shape->createShape(TOshapes[0]);
    shape->measure();
    shape->init();
  }
  // create AABB
  lipo_AABB = std::make_shared<AABB>(shape->min, shape->max);

  // initlize the background prog
  shadowProg = std::make_shared<Program>();
  shadowProg->setVerbose(true);
  shadowProg->setShaderNames(
      resourceDirectory + "/shaders/lipo_shadow_vert.glsl",
      resourceDirectory + "/shaders/lipo_shadow_frag.glsl");
  shadowProg->init();
  shadowProg->addUniform("M");
  shadowProg->addUniform("V");
  shadowProg->addUniform("P");
  shadowProg->addAttribute("vertPos");
  shadowProg->addAttribute("vertTex");
  shadowProg->addAttribute("vertNor");
}

// we need this to draw and transform the AABB
// TODO create own model matrix to reposition based on local pos and draw
void Lipo::draw(std::shared_ptr<Program> prog,
                std::shared_ptr<MatrixStack> Model, Drone &drone) {
  // std::cout << "Drawing the lipo at "  << position.x << " " <<  position.y <<
  // " " << position.z<< '\n';
  lipo_texture->bind(prog->getUniform("Texture0"));
  Model->pushMatrix();
  Model->translate(
      glm::vec3(position.x, sin(glfwGetTime()) * .5f + position.y, position.z));
  Model->rotate(glfwGetTime(), glm::vec3(0, 1, 0));
  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                     value_ptr(Model->topMatrix()));
  lipo_AABB->transform(Model->topMatrix());
  if (render) {
    shape->draw(prog);
  }
  Model->popMatrix();
}


void Lipo::update(float dt, Drone &drone) {
  // charge drone battery;
  drone.battery += 25.0f;
	drone.setBatteriesCollected(drone.getBatteriesCollected()+1);
  newRandPosition();
  return;
}

void Lipo::chargeBattery(Drone drone) { drone.battery = 100.0f; }

void Lipo::newRandPosition() {
  position = possible_locations[(rand() % possible_locations.size())];
}

std::shared_ptr<AABB> Lipo::getAABB() { return lipo_AABB; }
