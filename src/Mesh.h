// Mesh.h
#pragma once
#include "Shape.h"
#include "AABB.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct Mesh {
  std::vector<std::shared_ptr<Shape>> shapes;
  glm::vec3 gMin, gMax;
};
