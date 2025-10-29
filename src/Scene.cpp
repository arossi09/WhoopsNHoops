#include "Scene.h"
#include "Program.h"
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

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

    sceneObj.shape = rm.getShape(sceneObj.name, "../resources/" + modelFile);
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

// this function is needed to draw each model
void Scene::draw(std::shared_ptr<Program> prog, const glm::mat4 &viewProj) {
  for (auto &obj : sceneObjects) {
    obj.texture->bind(prog->getUniform("Texture0"));
    obj.draw(prog, viewProj);
  }
}
