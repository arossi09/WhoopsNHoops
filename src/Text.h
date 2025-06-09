#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "Program.h"
#include <map>
struct Character {
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
    glm::vec2 UVOffset; // bottom-left corner in atlas (0-1 range)
    glm::vec2 UVSize;   // width/height in atlas (0-1 range)
};
namespace Text {
    extern unsigned int VAO, VBO, atlasTextureID;
    void load_characters(std::map<char, Character>& Characters);
    void RenderText(std::shared_ptr<Program> prog, std::string text, float x,
            float y, float scale, glm::vec3 color, const std::map<char, Character> &Characters);

}
