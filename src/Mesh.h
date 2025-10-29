// Mesh.h
#pragma once
#include "Shape.h"
#include "AABB.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct Mesh {
  std::vector<std::shared_ptr<Shape>> shapes;
  std::vector<std::shared_ptr<AABB>> aabbs;
  glm::vec3 gMin, gMax;
};
