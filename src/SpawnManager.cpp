#include "SpawnManager.h"

glm::vec3 SpawnManager::get_new_spawn(glm::vec3 old_spawn) {
  glm::vec3 new_spawn{};
  for (auto &spawn : possible_spawns) {
    if (!spawn.used){
      new_spawn = spawn.pos;
			spawn.used = true;
			break;
		}
  }
  for (auto &spawn : possible_spawns) {
    if (old_spawn == spawn.pos)
      spawn.used = false;
  }
  return new_spawn;
}
