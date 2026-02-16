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

  tree_texture = std::make_shared<Texture>();
  tree_texture->setFilename(resourceDirectory + "/billboard_tree.png");
  tree_texture->init();
  tree_texture->setUnit(0);
  tree_texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  tree_texture->setFiltering(GL_NEAREST, GL_NEAREST);
  // load mountain obj
  std::vector<tinyobj::shape_t> TOshapesA;
  std::vector<tinyobj::material_t> objMaterialsA;
  std::string errStr;
  bool rc = tinyobj::LoadObj(TOshapesA, objMaterialsA, errStr,
                             (resourceDirectory + "/landscape.obj").c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {
    mountain_obj = std::make_shared<Shape>();
    mountain_obj->createShape(TOshapesA[0]);
    mountain_obj->measure();
    mountain_obj->init();
  }

  std::vector<tinyobj::shape_t> TOshapes;
  std::vector<tinyobj::material_t> objMaterials;
  rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                        (resourceDirectory + "/quad.obj").c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {
    tree_obj = std::make_shared<Shape>();
    tree_obj->createShape(TOshapes[0]);
    tree_obj->measure();
    tree_obj->init();
  }
  // store triangle points of of mountain obj
  std::vector<glm::vec3> mountain_tris = mountain_obj->getTris();
  sampleTreePoints(0.2f, 0.6f, mountain_tris);
  // load in tree obj

  return 0;
}

// we need this function to be able to sample points on the
// mountain to draw trees given a threshold
void Mountain::sampleTreePoints(float thresh_lower, float thresh_higher,
                                std::vector<glm::vec3> tris) {
  tree_samples.clear();
  float density = 0.08f;

  for (int i = 0; i < tris.size(); i += 3) {
    glm::vec3 v0 = tris[i];
    glm::vec3 v1 = tris[i + 1];
    glm::vec3 v2 = tris[i + 2];

    float ay = (v0.y + v1.y + v2.y) / 3.0f;
    if (ay < thresh_lower || ay > thresh_higher)
      continue;

    if ((float)rand() / RAND_MAX < density) {
      glm::vec3 centroid = (v0 + v1 + v2) / 3.0f;
      tree_samples.push_back(centroid);
    }
  }

  return;
}

// this function will draw the mountain along with the sampled trees
int Mountain::draw(std::shared_ptr<Program> prog,
                   std::shared_ptr<MatrixStack> Model, glm::vec3 cameraPos) {

  // we need to draw the mountain
  mountain_texture->bind(prog->getUniform("Texture0"));
  Model->pushMatrix();
  Model->translate(glm::vec3(-4, -30, 0));
  Model->scale(glm::vec3(300, 300, 300));
  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                     value_ptr(Model->topMatrix()));
  mountain_obj->draw(prog);
  tree_texture->bind(prog->getUniform("Texture0"));
  for (int i = 0; i < tree_samples.size(); i++) {
		//we need to transfer the tree Points to world space before
		//calculating the rotation
		glm::vec3 worldTreePos =
    glm::vec3(Model->topMatrix() * glm::vec4(tree_samples[i], 1.0));
		//caclulate the angle of rotation to face the camera
    glm::vec3 toCamera = cameraPos - worldTreePos;
    float angle = atan2(toCamera.x, toCamera.z);
    Model->pushMatrix();
			//move the tree to the correct sampled point, rotate and draw
			Model->translate(tree_samples[i]);
			Model->rotate(angle, glm::vec3(0, 1, 0));
			Model->scale(glm::vec3(0.3, 0.3, 0.3));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
												 value_ptr(Model->topMatrix()));
			tree_obj->draw(prog);
    Model->popMatrix();
  }
  Model->popMatrix();

  return 0;
}

void Mountain::setResourceDir(const std::string &resourceDir) {
  resourceDirectory = resourceDir;
}
