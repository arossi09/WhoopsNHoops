#ifndef SKYBOX_H
#define SKYBOX_H

#include "Program.h"
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

class Skybox {
public:
  void setFaces(std::vector<std::string> &aFaces);
  void init();
  void draw();
  unsigned int getTexId();
  ~Skybox();

private:
  void loadCubeMap();
  unsigned int VAO, VBO, EBO, cubemapTexture;
  std::array<unsigned int, 36> skyboxIndices;
  std::array<float, 24> skyboxVertices;
  glm::vec3 position;
  std::string resourceDir = "../resources";
  std::vector<std::string> faces;
};

#endif
