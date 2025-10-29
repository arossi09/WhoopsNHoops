#pragma once
#include <iostream>
#include "AABB.h"
class Shape;
class Texture;

class ResourceManager {
public:
  std::shared_ptr<Shape> getShape(const std::string &name,
                                  const std::string &path);
  std::shared_ptr<Texture> getTexture(const std::string &name, const std::string &path);

	struct Mesh{
		std::vector<std::shared_ptr<Shape>> shapes;
		std::vector<std::shared_ptr<AABB>> aabbs;
		glm::vec3 gMin, gMax;
	};

private:
  std::unordered_map<std::string, std::shared_ptr<Shape>> shapeCache;
  std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
};
