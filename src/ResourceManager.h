#pragma once
#include "AABB.h"
#include "Mesh.h"
#include <iostream>
class Shape;
class Texture;

class ResourceManager {
public:

  std::shared_ptr<Mesh> getMesh(const std::string &name,
                                const std::string &path);
  std::shared_ptr<Texture> getTexture(const std::string &name,
                                      const std::string &path);

private:
  std::unordered_map<std::string, std::shared_ptr<Mesh>> meshCache;
  std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
};
