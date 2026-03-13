#include "plane.h"


// plane takes the div amount width/size of the plane
// and the position which is defualted to the origin
Plane::Plane(int div, int width, glm::vec3 position) {

  this->div = div;
  this->width = width;
  this->Position = position;

  initPlane();
}

// sets the Div amount of the plane
void Plane::setDiv(int div) {
  if (this->div != div) {
    this->div = div;
    needsUpdate = true;
  }
}

// sets teh width/sie of the plane
void Plane::setWidth(int width) {
  if (this->width != width) {
    this->width = width;
    needsUpdate = true;
  }
}

// initializes the plane to flat surface
void Plane::initPlane() {

  vertices.clear();
  indices.clear();

  float triangleSide = width / div;
  for (int row = 0; row < div + 1; row++) {
    for (int col = 0; col < div + 1; col++) {
      glm::vec3 crntVec =
          glm::vec3(col * triangleSide, 0.0f, row * -triangleSide);
      vertices.push_back(crntVec.x);
      vertices.push_back(crntVec.y);
      vertices.push_back(-crntVec.z);
    }
  }

  for (int row = 0; row < div; row++) {
    for (int col = 0; col < div; col++) {
      int index = row * (div + 1) + col;
      // top triangle
      indices.push_back(index);
      indices.push_back(index + (div + 1) + 1);
      indices.push_back(index + (div + 1));
      // Bot triangle
      indices.push_back(index);
      indices.push_back(index + 1);
      indices.push_back(index + (div + 1) + 1);
    }
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  // bind the vertex object array for the vertices
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
               vertices.data(), GL_STATIC_DRAW);

  // bind the element array buffer for the order of elements
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  needsUpdate = false;
}

// draws the plane
void Plane::draw() {
  if (needsUpdate) {
    this->initPlane();
  }

  glPatchParameteri(GL_PATCH_VERTICES, 3);
  glBindVertexArray(VAO);
  glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

Plane::~Plane() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
