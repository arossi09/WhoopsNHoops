// in charge of holding entities and operating on them
#include "Entity.h"
#include <iostream>

class EntityProcess {
public:
  std::vector<std::shared_ptr<Entity>> entities;

  // we need to loop through entites and draw them
  void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model, Drone &drone) {
    for (auto const &ent : entities) {
			
      ent->draw(prog, Model, drone);
    }
  }

  void add(std::shared_ptr<Entity> entity) { entities.push_back(entity); }

  // we need to loop through entities and see if they intersect with drone in
  // order to do their update state
  void update(float dt, Drone &drone) {
    AABB droneAABB = drone.getAABB();
    for (int i = 0; i < entities.size(); i++) {
      if (entities[i] && entities[i]->getAABB()) {
        if (entities[i]->getAABB()->intersects(droneAABB)) {
          entities[i]->update(dt, drone);
        }
      } else {
        std::cout << "UPDATE::ENTITIES: AABB is NULL!" << std::endl;
      }
    }

		//TODO add loop to check if battery needs respawn..
  }
};
