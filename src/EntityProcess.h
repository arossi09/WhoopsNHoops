// in charge of holding entities and operating on them
#include "Entity.h"
#include "SoundManager.h"
#include "SpawnManager.h"
#include "SpecialTrickBonus.h"
#include <iostream>
#include <memory>

class EntityProcess {
public:
  SpawnManager spawn_manager{};

  std::vector<std::shared_ptr<Entity>> entities;

  // we need to loop through entites and draw them
  void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model,
            Drone &drone) {
    for (auto const &ent : entities) {

      ent->draw(prog, Model, drone);
    }
  }

  void add(std::shared_ptr<Entity> entity) { entities.push_back(entity); }

	//used for initlizing all entities
  void init() {
    for (int i = 0; i < entities.size(); i++) {
      if (entities[i]->getNeedRespawn()) {
        if (dynamic_cast<SpecialTrickBonus *>(entities[i].get())) {
          glm::vec3 curr_pos = entities[i]->getPos();
          entities[i]->setPos(spawn_manager.get_special_new_spawn(curr_pos));
          entities[i]->setNeedRespawn(false);
        } else {
          glm::vec3 curr_pos = entities[i]->getPos();
          entities[i]->setPos(spawn_manager.get_new_spawn(curr_pos));
          entities[i]->setNeedRespawn(false);
        }
      }
    }
  }

  // we need to loop through entities and see if they intersect with drone in
  // order to do their update state
  void update(float dt, Drone &drone, SoundManager &sm) {
    AABB droneAABB = drone.getAABB();
    for (int i = 0; i < entities.size(); i++) {
      if (entities[i] && entities[i]->getAABB()) {
        if (entities[i]->getAABB()->intersects(droneAABB)) {
          entities[i]->update(dt, drone);
        }
        // we need pull new spawn for entity from spawn manager if
        // the entity needs a respawn
        if (entities[i]->getNeedRespawn()) {
          if (dynamic_cast<SpecialTrickBonus *>(entities[i].get())) {
          	sm.play(SPECIAL);
            glm::vec3 curr_pos = entities[i]->getPos();
            entities[i]->setPos(spawn_manager.get_special_new_spawn(curr_pos));
            entities[i]->setNeedRespawn(false);
          } else {
          	sm.play(BATTERY_PICKUP);
            glm::vec3 curr_pos = entities[i]->getPos();
            entities[i]->setPos(spawn_manager.get_new_spawn(curr_pos));
            entities[i]->setNeedRespawn(false);
          }
        }
      } else {
        std::cout << "UPDATE::ENTITIES: AABB is NULL!" << std::endl;
      }
    }
  }
};
