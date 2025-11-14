#ifndef HUD_H
#define HUD_H

#include "Program.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct HUDSprite {
  //Texture texture;
  glm::vec2 position;
  glm::vec2 size;
  glm::vec3 color;
  float value;
  float time;
};

class Hud {

public:
  void init();
  void addSprite(const HUDSprite &sprite);
  void removeSprite(size_t index);
  void update(float dt);
  void draw();
	void setScreenSize(int width, int height);

private:
	int screenWidth = 800;
	int screenHeight = 600;
	std::shared_ptr<Program> hudShader;
  std::vector<HUDSprite> sprites;
  GLuint VAO, VBO, EBO;
	std::string resourceDir = "../resources";

  glm::mat4 orthoProj= glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);


  void setupQuad();
};

#endif
