#include "SpawnManager.h"

// grab a new spawn from the pool of spawns
glm::vec3 SpawnManager::get_new_spawn(glm::vec3 old_spawn) {
  std::vector<int> available;

  for (int i = 0; i < possible_spawns.size(); ++i) {
    if (!possible_spawns[i].used)
      available.push_back(i);
  }

  if (available.empty())
    return old_spawn;

  int random_index = available[rand() % available.size()];
  possible_spawns[random_index].used = true;

  for (auto &spawn : possible_spawns) {
    if (old_spawn == spawn.pos)
      spawn.used = false;
  }
  return possible_spawns[random_index].pos;
}

glm::vec3 SpawnManager::get_special_new_spawn(glm::vec3 old_spawn) {
  std::vector<int> available;

  for (int i = 0; i < possible_special_spawns.size(); ++i) {
    if (!possible_special_spawns[i].used)
      available.push_back(i);
  }

  if (available.empty())
    return old_spawn;

  int random_index = available[rand() % available.size()];
  possible_special_spawns[random_index].used = true;

  for (auto &spawn : possible_special_spawns) {
    if (old_spawn == spawn.pos)
      spawn.used = false;
  }
  return possible_special_spawns[random_index].pos;
}
