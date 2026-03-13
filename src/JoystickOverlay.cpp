#include "JoystickOverlay.h"
#include "Texture.h"

void JoystickOverlay::init() {
  // Initlize the textures and arrays for the background and moving stick
  // element
  float background_verticies[] = {0.f, 1.f, 1.0f, 1.0f,  // top right
                                  1.f, 1.f, 1.0f, 0.0f,  // bottom right
                                  1.f, 0.f, 0.0f, 0.0f,  // bottom left
                                  0.f, 0.f, 0.0f, 1.0f}; // top left

  unsigned int indicies[] = {0, 1, 3, 1, 2, 3};
  glGenVertexArrays(1, &joystick_VAO);
  glGenBuffers(1, &joystick_VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(joystick_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, joystick_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(background_verticies),
               background_verticies, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // texture cordinates
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // create the texture for the joystick background and joystick

  joystick_background_texture = std::make_shared<Texture>();
  joystick_background_texture->setFilename(resource_directory +
                                           "/joystick_background.png");
  joystick_background_texture->init();
  joystick_background_texture->setUnit(1);
  joystick_background_texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  joystick_background_texture->setFiltering(GL_NEAREST, GL_NEAREST);

  joystick_foreground_texture = std::make_shared<Texture>();
  joystick_foreground_texture->setFilename(resource_directory +
                                           "/joystick_foreground.png");
  joystick_foreground_texture->init();
  joystick_foreground_texture->setUnit(1);
  joystick_foreground_texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  joystick_foreground_texture->setFiltering(GL_NEAREST, GL_NEAREST);

  joystickShader = std::make_shared<Program>();
  joystickShader->setVerbose(true);
  joystickShader->setShaderNames(
      resource_directory + "/shaders/joystickVS.glsl",
      resource_directory + "/shaders/joystickFS.glsl");
  joystickShader->init();
  joystickShader->addUniform("M");
  joystickShader->addUniform("Texture0");
}

void JoystickOverlay::setResourceDir(const std::string &aResourceDir) {
  resource_directory = aResourceDir;
}

void JoystickOverlay::draw() {
  joystickShader->bind();
  glDisable(GL_DEPTH_TEST);
  glBindVertexArray(joystick_VAO);
  glm::mat4 background_model(1.0f);
  glm::mat4 foreground_model(1.0f);
  // draw the joystick background
  background_model = glm::scale(background_model, glm::vec3(.15f, .25f, 1.f));
  background_model =
      glm::translate(background_model, glm::vec3(global_position, 0.f));

  joystick_background_texture->bind(joystickShader->getUniform("Texture0"));
  glUniformMatrix4fv(joystickShader->getUniform("M"), 1, GL_FALSE,
                     glm::value_ptr(background_model));
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  // draw the joystick foreground
  foreground_model = glm::scale(foreground_model, glm::vec3(.15f, .25f, 1.f));
  //foreground_model = glm::scale(
      //TODO fix the order of scale and translate foreground_model, glm::vec3(joystick_scale, joystick_scale, 1.f));
  foreground_model =
      glm::translate(foreground_model, glm::vec3(global_position, 0.f));
  foreground_model =
      glm::translate(foreground_model, glm::vec3(joystick_position, 0.f));
  joystick_foreground_texture->bind(joystickShader->getUniform("Texture0"));
  glUniformMatrix4fv(joystickShader->getUniform("M"), 1, GL_FALSE,
                     glm::value_ptr(foreground_model));
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glEnable(GL_DEPTH_TEST);
  joystickShader->unbind();
}
void JoystickOverlay::setGlobalPosition(float screen_x, float screen_y) {
  global_position.x = screen_x;
  global_position.y = screen_y;
}

void JoystickOverlay::updateJoystickPosition(float joystick_position_x,
                                             float joystick_position_y) {
  // update a position variable which translates the verticies in the draw call
  joystick_position.x = joystick_position_x / 6;
  joystick_position.y = joystick_position_y / 4;
  joystick_scale = fmax(joystick_position_y + joystick_position_x, 1.0f);
}
