#ifndef SCENE_H
#define SCENE_H

#include "Mesh.h"
#include "Program.h"
#include "ResourceManager.h"
#include "Shape.h"
#include "Texture.h"
#include <glad/glad.h>
#include <iostream>
#include <memory>

#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Shape;
class Texture;
class AABB;

class Scene {
public:
  void load(const std::string &path, ResourceManager &rm);

  void draw(std::shared_ptr<Program> prog, const glm::mat4 &viewProj);

private:
  // these are the scene objects that are needed to encapsulate
  // all objects data and transforms
  struct SceneObject {
    std::string name;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f);
    std::shared_ptr<AABB> aabb;

    void draw(std::shared_ptr<Program> prog, const glm::mat4 &parent) {
      glm::mat4 model = glm::translate(parent, position);
      model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
      model = glm::scale(model, scale);
      glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                         glm::value_ptr(model));
      for (auto &shape : mesh->shapes) {
        shape->draw(prog);
      }
    }
  };

  std::vector<SceneObject> sceneObjects;
};
#endif
