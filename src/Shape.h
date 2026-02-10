// this class is just a wrapper for the loaded obj via the tiny obj
// it allows us to draw the shape given a shader program
#pragma once

#ifndef LAB471_SHAPE_H_INCLUDED
#define LAB471_SHAPE_H_INCLUDED

#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include <vector>

class Program;

class Shape {

public:
  void createShape(tinyobj::shape_t &shape);
  void init();
  void measure();
  void draw(const std::shared_ptr<Program> prog) const;
  std::vector<glm::vec3> getTris();

  glm::vec3 min = glm::vec3(0);
  glm::vec3 max = glm::vec3(0);

private:
  std::vector<unsigned int> eleBuf;
  std::vector<float> posBuf;
  std::vector<float> norBuf;
  std::vector<float> texBuf;
  unsigned int eleBufID = 0;
  unsigned int posBufID = 0;
  unsigned int norBufID = 0;
  unsigned int texBufID = 0;
  unsigned int vaoID = 0;
};

#endif // LAB471_SHAPE_H_INCLUDED
