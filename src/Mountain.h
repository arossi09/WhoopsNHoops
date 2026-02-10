#ifndef MOUNTAIN_H
#define MOUNTAIN_H
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Texture.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
class Mountain {
public:
  // we need this function to initlize the mountain obj
  // as well as the list of samples for trees
  int init();
  void setResourceDir(const std::string &resourceDir);
  // given a shader program draws the mountain
  int draw(std::shared_ptr<Program> prog, std::shared_ptr<Program> bill_prog,
           std::shared_ptr<MatrixStack> Model);

private:
	unsigned int VBO;
	unsigned int VAO;
  void sampleTreePoints(float thresh, std::vector<glm::vec3> tris);
  std::shared_ptr<Shape> mountain_obj;
  std::shared_ptr<Texture> mountain_texture;
  std::shared_ptr<Shape> tree_obj;
  std::shared_ptr<Texture> tree_texture;
  std::string resourceDirectory;
  std::vector<float> tree_samples; // list of tree sample points
};
#endif
