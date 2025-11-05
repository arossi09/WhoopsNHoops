#include "Scene.h"
#include "Program.h"
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

void SceneObject::draw(std::shared_ptr<Program> prog, const glm::mat4 &parent) {
  glm::mat4 model = glm::translate(parent, position);
  model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
  model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
  model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
  model = glm::scale(model, scale);

  float center_x = (mesh->gMax.x + mesh->gMin.x) / 2;
  float center_y = (mesh->gMax.y + mesh->gMin.y) / 2;
  float center_z = (mesh->gMax.z + mesh->gMin.z) / 2;

  float largest_extent = std::max(
      std::max((mesh->gMax.x - mesh->gMin.x), (mesh->gMax.y - mesh->gMin.y)),
      (mesh->gMax.z - mesh->gMin.z));
  float scale = 2.0 / largest_extent;
  model = glm::translate(model, glm::vec3(-center_x, -center_y, -center_z));
  model = glm::scale(model, glm::vec3(scale, scale, scale));

  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(model));
  for (auto &shape : mesh->shapes) {
    shape->draw(prog);
  }
}

// this function is used to create AABB objects for each
// shape and transform them appropriatly
void SceneObject::setupColliders() {
  colliders.clear();
  glm::mat4 model = glm::mat4(1.0f);


  model = glm::translate(model, glm::vec3(0, 2, 0));					// this needs to be fixed the reason for it is beacuse of draw taking in parent that is modified
  model = glm::scale(model, glm::vec3(4, 4, 4));

  model = glm::translate(model, position);
  model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
  model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
  model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
  model = glm::scale(model, scale);

	//for resize and centering
  float center_x = (mesh->gMax.x + mesh->gMin.x) / 2;
  float center_y = (mesh->gMax.y + mesh->gMin.y) / 2;
  float center_z = (mesh->gMax.z + mesh->gMin.z) / 2;

  float largest_extent = std::max(
      std::max((mesh->gMax.x - mesh->gMin.x), (mesh->gMax.y - mesh->gMin.y)),
      (mesh->gMax.z - mesh->gMin.z));
  float scale = 2.0 / largest_extent;
  model = glm::translate(model, glm::vec3(-center_x, -center_y, -center_z));
  model = glm::scale(model, glm::vec3(scale, scale, scale));

  for (auto &shape : mesh->shapes) {
    auto box = std::make_shared<AABB>(shape->min, shape->max);
    box->transform(model);
    colliders.push_back(box);
  }
}

// this function is used to load in scene from .json file
void Scene::load(const std::string &path, ResourceManager &rm) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open scene file: " << path << std::endl;
    return;
  }

  json data;
  file >> data;

  for (auto &obj : data["objects"]) {
    SceneObject sceneObj;

    sceneObj.name = obj.value("name", "unamed");

    std::string modelFile = obj.value("model", "");
    std::string textureFile = obj.value("texture", "");

    sceneObj.mesh = rm.getMesh(sceneObj.name, "../resources/" + modelFile);
    sceneObj.texture =
        rm.getTexture(sceneObj.name, "../resources/" + textureFile);

    auto pos = obj["position"];
    auto rot = obj["rotation"];
    auto scale = obj["scale"];

    sceneObj.position = glm::vec3(pos[0], pos[1], pos[2]);
    sceneObj.rotation = glm::radians(glm::vec3(rot[0], rot[1], rot[2]));
    sceneObj.scale = glm::vec3(scale[0], scale[1], scale[2]);
    sceneObjects.push_back(sceneObj);
  }
}

// this function is used to add colliders to the sceneobjects
// and add those colliders to the physics world object
void Scene::setupPhysics(PhysicsWorld &world) {
  for (auto &obj : sceneObjects) {
    obj.setupColliders();
    world.addSceneObject(obj);
  }
}

// this function is needed to draw each model
void Scene::draw(std::shared_ptr<Program> prog, const glm::mat4 &viewProj) {
  for (auto &obj : sceneObjects) {
    obj.texture->bind(prog->getUniform("Texture0"));
    obj.draw(prog, viewProj);
  }
}
