#include "Hud.h"
#include "MatrixStack.h"

void Hud::init() {
  float vertices[] = {0.f, 1.f, 1.0f, 1.0f,  // top right
                      1.f, 1.f, 1.0f, 0.0f,  // bottom right
                      1.f, 0.f, 0.0f, 0.0f,  // bottom left
                      0.f, 0.f, 0.0f, 1.0f}; // top left

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // texture cordinates
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  hudShader = std::make_shared<Program>();
  hudShader->setVerbose(true);
  hudShader->setShaderNames(resourceDir + "/shaders/hudVS.glsl",
                            resourceDir + "/shaders/hudFS.glsl");
  hudShader->init();
  hudShader->addUniform("P");
  hudShader->addUniform("M");
  hudShader->addUniform("Texture0");
  hudShader->addUniform("uFilled");
}

void Hud::addSprite(const HUDSprite &sprite) { sprites.push_back(sprite); }

void Hud::draw() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  hudShader->bind();
  glUniform1f(hudShader->getUniform("uFilled"), hudDisplayFill);
  glUniformMatrix4fv(hudShader->getUniform("P"), 1, GL_FALSE,
                     value_ptr(orthoProj));
  glBindVertexArray(VAO);
  auto Model = std::make_shared<MatrixStack>();
  for (const auto &sprite : sprites) {
    sprite.texture->bind(hudShader->getUniform("Texture0"));
    glm::mat4 M(1.0f);
    M = glm::translate(M, glm::vec3(sprite.position, 0.f));
    M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(0, 0, 1));
    M = glm::scale(M, glm::vec3(sprite.size, 1.f));
    glUniformMatrix4fv(hudShader->getUniform("M"), 1, GL_FALSE, value_ptr(M));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
  hudShader->unbind();
  glDisable(GL_BLEND);
}

void Hud::setTargetFill(float amount) {
  hudTargetFill= glm::clamp(amount, 0.0f, 1.0f);
}

void Hud::update(float dt){
	float speed = 2;
	hudDisplayFill = glm::mix(hudDisplayFill, hudTargetFill, dt*speed);
}


void Hud::setScreenSize(int width, int height) {
  screenWidth = width;
  screenHeight = height;
}
