#include "ResourceManager.h"
#include "Shape.h"
#include "Texture.h"

#include <glad/glad.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

std::shared_ptr<Mesh> ResourceManager::getMesh(const std::string &name,
                                               const std::string &path) {

  // check if the mesh is already cached
  if (meshCache.count(name))
    return meshCache[name];

  auto mesh = std::make_shared<Mesh>();
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  glm::vec3 minBounds = glm::vec3(std::numeric_limits<float>::max());
  glm::vec3 maxBounds = glm::vec3(-std::numeric_limits<float>::max());
  std::string err;
  bool rc = tinyobj::LoadObj(shapes, materials, err, path.c_str());
  if (!rc)
    std::cerr << err << std::endl;

  for (auto &toShape : shapes) {
    auto shape = std::make_shared<Shape>();
    shape->createShape(toShape);
    shape->measure();
    shape->init();
    mesh->shapes.push_back(shape);

    minBounds.x = std::min(minBounds.x, shape->min.x);
    minBounds.y = std::min(minBounds.y, shape->min.y);
    minBounds.z = std::min(minBounds.z, shape->min.z);

    maxBounds.x = std::max(maxBounds.x, shape->max.x);
    maxBounds.y = std::max(maxBounds.y, shape->max.y);
    maxBounds.z = std::max(maxBounds.z, shape->max.z);
  }

  mesh->gMin = minBounds;
  mesh->gMax = maxBounds;
  // cache the mesh
  meshCache[name] = mesh;
  return mesh;
}
std::shared_ptr<Texture> ResourceManager::getTexture(const std::string &name,
                                                     const std::string &path) {
  if (textureCache.count(name))
    return textureCache[name];

  auto tex = std::make_shared<Texture>();
  tex->setFilename(path);
  tex->init();
  tex->setUnit(0);
  tex->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  tex->setFiltering(GL_NEAREST, GL_NEAREST);
  textureCache[name] = tex;
  return tex;
}
