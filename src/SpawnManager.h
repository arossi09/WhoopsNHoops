// this class is in charge of holding valid locations to
// spawn new entities
#ifndef SPAWN_MANAGER_H
#define SPAWN_MANAGER_H
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#define BELOW_CARGO glm::vec3(-16, 15, 13)
#define INSIDE_CARGO glm::vec3(-16, 15, 13)
#define ABOVE_CARGO glm::vec3(-20, 18, 13)
#define NEAR_WIRE glm::vec3(-20, 14, -10)
#define INSIDE_CARGO_ON_GARAGE glm::vec3(.5, 7, 11)
#define OUTSIDE_WINDOW glm::vec3(8, 2, 6)
#define BEHIND_GARAGE glm::vec3(1, 1, 22)
#define INSIDE_CARGO_ON_FLOOR glm::vec3(-13, 0, 20)
#define HILL_CRANE glm::vec3(20, 45, 35)
#define HILL_SCAFFOLDING_ONE glm::vec3(-21, 44, 38)
#define HILL_SCAFFOLDING_TWO glm::vec3(-21, 34, 48)
#define ABANDONED_HOLE_LOWER glm::vec3(-2, 35.5, 45)
#define ABANDONED_HOLE_UPPER glm::vec3(-8, 41, 43)
struct SpawnPoint {
  glm::vec3 pos;
  bool used{false};
};

class SpawnManager {

public:
  // this functions needs to grab a new spawn based off the ones
  // left in pool and set the old spawn to open
  glm::vec3 get_new_spawn(glm::vec3 old_spawn);

private:
  // this vector holds a the possible_spawns as their
  // spawnPoint data structure defined at the top
  std::vector<SpawnPoint> possible_spawns = {{glm::vec3(-21, 6, 3)},
                                             {glm::vec3(-9, 0, -16)},
                                             {glm::vec3(0, 4, -4)},
                                             {glm::vec3(1, 2, 5)},
                                             {BELOW_CARGO},
                                             {INSIDE_CARGO},
                                             {ABOVE_CARGO},
                                             {NEAR_WIRE},
                                             {INSIDE_CARGO_ON_GARAGE},
                                             {OUTSIDE_WINDOW},
                                             {BEHIND_GARAGE},
                                             {INSIDE_CARGO_ON_FLOOR},
                                             {HILL_CRANE},
                                             {HILL_SCAFFOLDING_ONE},
                                             {HILL_SCAFFOLDING_TWO},
                                             {ABANDONED_HOLE_LOWER},
                                             {ABANDONED_HOLE_UPPER}};

};
#endif
