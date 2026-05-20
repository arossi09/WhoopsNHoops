#include "skybox.h"


#include "stb_image.h"

void Skybox::setFaces(std::vector<std::string> &aFaces) {
  this->faces = aFaces;
}

// this function is needed to initilize the cubemap
// vertices and indices
void Skybox::init() {
  skyboxVertices = {                     //   Coordinates
                    -1.0f, -1.0f, 1.0f,  //        7--------6
                    1.0f,  -1.0f, 1.0f,  //       /|       /|
                    1.0f,  -1.0f, -1.0f, //      4--------5 |
                    -1.0f, -1.0f, -1.0f, //      | |      | |
                    -1.0f, 1.0f,  1.0f,  //      | 3------|-2
                    1.0f,  1.0f,  1.0f,  //      |/       |/
                    1.0f,  1.0f,  -1.0f, //      0--------1
                    -1.0f, 1.0f,  -1.0f};

  skyboxIndices = {// Right
                   1, 2, 6, 6, 5, 1,
                   // Left
                   0, 4, 7, 7, 3, 0,
                   // Top
                   4, 5, 6, 6, 7, 4,
                   // Bottom
                   0, 3, 2, 2, 1, 0,
                   // Back
                   0, 1, 5, 5, 4, 0,
                   // Front
                   3, 7, 6, 6, 2, 3};

  // we need to bind & setup VAO, VBO, & EBO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float),
               skyboxVertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               skyboxIndices.size() * sizeof(unsigned int),
               skyboxIndices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  loadCubeMap(); // maybe offload this to manual public and pass face to change
                 // on the fly
}

// we need this function for loading in the textures from the faces
// given and returning the textureID to bind later in the draw call
void Skybox::loadCubeMap() {
  glGenTextures(1, &cubemapTexture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	stbi_set_flip_vertically_on_load(false);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data = stbi_load((resourceDir + faces[i]).c_str(), &width,
                                    &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap tex failed to load path" << resourceDir + faces[i]
                << std::endl;
      stbi_image_free(data);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::draw() {
  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
  glDrawElements(GL_TRIANGLES, skyboxIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

unsigned int Skybox::getTexId() { return cubemapTexture; }

Skybox::~Skybox() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}
