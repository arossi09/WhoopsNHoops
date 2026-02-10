#include "Mountain.h"
#include <cstdlib>

// we need this function to initlize the obj's and textures
// of the mountain and trees as well as load the points of the
// mountain for sampling
int Mountain::init() {
  // load in mountain texture
  mountain_texture = std::make_shared<Texture>();
  mountain_texture->setFilename(resourceDirectory + "/grass.png");
  mountain_texture->init();
  mountain_texture->setUnit(0);
  mountain_texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  mountain_texture->setFiltering(GL_NEAREST, GL_NEAREST);
  // load mountain obj
  std::vector<tinyobj::shape_t> TOshapes;
  std::vector<tinyobj::material_t> objMaterials;
  std::string errStr;
  bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                             (resourceDirectory + "/landscape.obj").c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {
    mountain_obj = std::make_shared<Shape>();
    mountain_obj->createShape(TOshapes[0]);
    mountain_obj->measure();
    mountain_obj->init();
  }
  // store triangle points of of mountain obj
  std::vector<glm::vec3> mountain_tris = mountain_obj->getTris();
  sampleTreePoints(2.0f, mountain_tris);
  // load in tree obj
  // load in tree texture

  return 0;
}

// we need this function to be able to sample points on the
// mountain to draw trees given a threshold
void Mountain::sampleTreePoints(float thresh, std::vector<glm::vec3> tris) {
	tree_samples.clear();
  /*for (int i = 0; i < tris.size(); i += 3) {
    // need to put these in world cordiantes
    glm::vec3 v0 = tris[i];
    glm::vec3 v1 = tris[i + 1];
    glm::vec3 v2 = tris[i + 2];

    // load point in model space so later in the draw call we can move
    // the point related to where the mountain is transformed
    float ax = (v0.x + v1.x + v2.x) / 3.0f;
    float ay = (v0.y + v1.y + v2.y) / 3.0f;
    float az = (v0.z + v1.z + v2.z) / 3.0f;
    if (ay >= thresh) {
      tree_samples.push_back(ax);
      tree_samples.push_back(ay);
      tree_samples.push_back(az);
    }
  }*/

	tree_samples.push_back(1.0f);
	tree_samples.push_back(1.0f);
	tree_samples.push_back(1.0f);

  // send the tree_samples to a GPU buffer
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, tree_samples.size() * sizeof(float), tree_samples.data(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  return;
}

// this function will draw the mountain along with the sampled trees
int Mountain::draw(std::shared_ptr<Program> prog,
                   std::shared_ptr<Program> bill_prog,
                   std::shared_ptr<MatrixStack> Model) {

  // we need to draw the trees from the VBO bounded in init
  bill_prog->bind();
		Model->pushMatrix();
		glUniformMatrix4fv(bill_prog->getUniform("M"), 1, GL_FALSE,
											 value_ptr(Model->topMatrix()));
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
		Model->popMatrix();
  bill_prog->unbind();

  // we need to draw the mountain
  prog->bind();
  mountain_texture->bind(prog->getUniform("Texture0"));
  Model->pushMatrix();
  Model->translate(glm::vec3(-4, -30, 0));
  Model->scale(glm::vec3(300, 300, 300));
  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                     value_ptr(Model->topMatrix()));
  mountain_obj->draw(prog);
  Model->popMatrix();
  // TODO draw mountain
  // TODO loop through sampled points and draw trees with billboard program
  return 0;
}

void Mountain::setResourceDir(const std::string &resourceDir) {
  resourceDirectory = resourceDir;
}
