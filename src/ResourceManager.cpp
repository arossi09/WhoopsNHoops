#include "ResourceManager.h"
#include "Shape.h"
#include "Texture.h"

#include <glad/glad.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

std::shared_ptr<Shape> ResourceManager::getShape(const std::string &name,
                                                 const std::string &path) {

  if (shapeCache.count(name))
    return shapeCache[name];

  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  bool rc = tinyobj::LoadObj(shapes, materials, err, path.c_str());
  if (!rc)
    std::cerr << err << std::endl;

  auto shape = std::make_shared<Shape>();
  shape->createShape(shapes[0]);
  shape->measure();
  shape->init();
  shapeCache[name] = shape;
  return shape;
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
