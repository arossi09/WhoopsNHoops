#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "Program.h"
#include <map>
struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

namespace Text {
    extern unsigned int VAO, VBO;
    void load_characters(std::map<char, Character>& Characters);
    void RenderText(std::shared_ptr<Program> prog, std::string text, float x,
            float y, float scale, glm::vec3 color, std::map<char, Character> Characters);

}
