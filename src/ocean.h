#ifndef OCEAN_H
#define OCEAN_H

#define PLANE_DIV_AMOUNT 15
#define PLANE_WIDTH 20

#include "Program.h"
#include "plane.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Ocean {

public:
  void setResourceDir(const std::string &aResourceDir);
  void init();
  void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection,
              glm::vec3 viewPos, glm::vec3 lightDir, float g_Time);

private:
  Program waterShader;
  std::string resourceDir;
  Plane *plane = NULL;
  // light properties
  glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 diffuseColor = light_color * glm::vec3(0.5f, 0.5f, 0.5f);
  glm::vec3 ambientColor = diffuseColor * glm::vec3(0.4f, 0.4f, 0.4f);
  // material properties
  glm::vec3 material_color = glm::vec3(0.17f, 0.48f, 0.87f);
  glm::vec3 material_specular= glm::vec3(3.0117648f, 1.945098f, 0.8784314f);
  float material_shininess = 256.0f;
};

#endif
