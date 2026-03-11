#ifndef JOYSTICK_H
#define JOYSTICK_H
#include "Program.h"
#include "Texture.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
class JoystickOverlay {

public:
  void init();
  void setGlobalPosition(float joystick_position_x, float joystick_psoition_y);
  void updateJoystickPosition(float joystick_position_x,
                              float joystick_position_y);
  void draw();
  void setResourceDir(const std::string &aResourceDir);

private:
  GLuint background_VAO;
  GLuint background_VBO;
  GLuint EBO;
  glm::vec2 joystick_position{0};
  glm::vec2 global_position{0};
  std::shared_ptr<Texture> joystick_background_texture;
	std::shared_ptr<Texture>joystick_foreground_texture;
  GLuint joystick_VAO;
  GLuint joystick_VBO;
  std::string resource_directory;
  std::shared_ptr<Program> joystickShader;
};

#endif
