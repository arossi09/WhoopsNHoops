#include "ocean.h"

void Ocean::setResourceDir(const std::string &aResourceDir) {
  resourceDir = aResourceDir;
}
void Ocean::init() {
  // set up shader
  waterShader.setVerbose(true);
  waterShader.setShaderNames(resourceDir + "/shaders/waterVS.glsl",
                             resourceDir + "/shaders/waterFS.glsl",
														 "", // empty for g shader
                             resourceDir + "/shaders/waterTCS.glsl",
                             resourceDir + "/shaders/waterTES.glsl");
  waterShader.init();
  waterShader.addUniform("model");
  waterShader.addUniform("view");
  waterShader.addUniform("projection");
  waterShader.addUniform("dirLight");
  waterShader.addUniform("material");
  waterShader.addUniform("viewPos");
  waterShader.addUniform("skybox");
  waterShader.addUniform("g_Time");

  waterShader.addUniform("dirLight.direction");
  waterShader.addUniform("dirLight.ambient");
  waterShader.addUniform("dirLight.diffuse");
  waterShader.addUniform("dirLight.specular");

  // Material uniforms
  waterShader.addUniform("material.ambient");
  waterShader.addUniform("material.diffuse");
  waterShader.addUniform("material.specular");
  waterShader.addUniform("material.shininess");
  plane = new Plane(PLANE_DIV_AMOUNT, PLANE_WIDTH, glm::vec3(10, -2, 0));
}

// given the draw matricies renders the ocean
void Ocean::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection,
                   glm::vec3 viewPos, glm::vec3 lightDir, float g_Time) {
  waterShader.bind();
  glUniformMatrix4fv(waterShader.getUniform("model"), 1, GL_FALSE,
                     glm::value_ptr(model));
  glUniformMatrix4fv(waterShader.getUniform("view"), 1, GL_FALSE,
                     glm::value_ptr(view));
  glUniformMatrix4fv(waterShader.getUniform("projection"), 1, GL_FALSE,
                     glm::value_ptr(projection));
  glUniform3fv(waterShader.getUniform("viewPos"), 1, glm::value_ptr(viewPos));
  glUniform1f(waterShader.getUniform("g_Time"), g_Time);
  glUniform1i(waterShader.getUniform("skybox"), 0);

  // light settings
  glUniform3fv(waterShader.getUniform("dirLight.direction"), 1,
               glm::value_ptr(lightDir));
  glUniform3fv(waterShader.getUniform("dirLight.ambient"), 1,
               glm::value_ptr(ambientColor));
  glUniform3fv(waterShader.getUniform("dirLight.diffuse"), 1,
               glm::value_ptr(diffuseColor));
  glUniform3fv(waterShader.getUniform("dirLight.specular"), 1,
               glm::value_ptr(light_color));

  // material settings
  glUniform3fv(waterShader.getUniform("material.ambient"), 1,
               glm::value_ptr(material_color));
  glUniform3fv(waterShader.getUniform("material.diffuse"), 1,
               glm::value_ptr(material_color));
  glUniform3fv(waterShader.getUniform("material.specular"), 1,
               glm::value_ptr(material_specular));
  glUniform1f(waterShader.getUniform("material.shininess"), material_shininess);

  plane->draw();
  waterShader.unbind();
}
