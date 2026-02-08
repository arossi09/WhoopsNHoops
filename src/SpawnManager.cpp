#include "SpawnManager.h"

glm::vec3 SpawnManager::get_new_spawn(glm::vec3 old_spawn) {
	std::cout << "New Lipo\n";
  glm::vec3 new_spawn{};
  for (auto &spawn : possible_spawns) {
    std::cout << "Possible Spawn" << "\n"
              << "Position: " << spawn.pos.x << ", " << spawn.pos.y << ", "
              << spawn.pos.z << ", " << "\n"
              << "Used: " << spawn.used << "\n";
    if (!spawn.used){
			std::cout << "Choosing new spawn\n"<< "Position: " << spawn.pos.x << ", " << spawn.pos.y << ", "
				<< spawn.pos.z << ", " << "\n";
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
