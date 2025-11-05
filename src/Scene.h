#ifndef SCENE_H
#define SCENE_H

#include "Mesh.h"
#include "Program.h"
#include "ResourceManager.h"
#include "PhysicsWorld.h"
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

class SceneObject{
public:
    std::string name;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f);
    std::vector<std::shared_ptr<AABB>> colliders;

    void draw(std::shared_ptr<Program> prog, const glm::mat4 &parent);
    void setupColliders();

};

class Scene {
public:
  void load(const std::string &path, ResourceManager &rm);
	void setupPhysics(PhysicsWorld &world);
  void draw(std::shared_ptr<Program> prog, const glm::mat4 &viewProj);

private:

  // this function is needed to setup the colliders positions
  std::vector<SceneObject> sceneObjects;
};
#endif
