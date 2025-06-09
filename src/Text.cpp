#include "Text.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <vector>

unsigned int Text::VAO = 0;
unsigned int Text::VBO = 0;
unsigned int Text::atlasTextureID = 0;

void Text::load_characters(std::map<char, Character> &Characters) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init Freetype library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../resources/PxPlus_IBM_VGA_8x16.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font face" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // First, compute total width and max height for atlas
    int totalWidth = 0, maxHeight = 0;
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        totalWidth += face->glyph->bitmap.width;
        if (face->glyph->bitmap.rows > maxHeight)
            maxHeight = face->glyph->bitmap.rows;
    }

    unsigned char* atlasData = new unsigned char[totalWidth * maxHeight]();
    int xOffset = 0;

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        FT_Bitmap bmp = face->glyph->bitmap;

        for (int row = 0; row < bmp.rows; ++row) {
            memcpy(
                atlasData + (row * totalWidth + xOffset),
                bmp.buffer + (row * bmp.pitch),
                bmp.width
            );
        }

        Character character = {
            glm::ivec2(bmp.width, bmp.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x),
            glm::vec2((float)xOffset / totalWidth, 0.0f),
            glm::vec2((float)bmp.width / totalWidth, (float)bmp.rows / maxHeight)
        };

        Characters.insert({c, character});
        xOffset += bmp.width;
    }

    glGenTextures(1, &atlasTextureID);
    glBindTexture(GL_TEXTURE_2D, atlasTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, totalWidth, maxHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlasData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] atlasData;
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    float quad[6][4] = {
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 1.0f, 1.0f},

        {0.0f, 1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 0.0f}
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Text::RenderText(std::shared_ptr<Program> prog, std::string text, float x, float y, float scale, glm::vec3 color, const std::map<char, Character>& Characters) {
    glUniform3f(prog->getUniform("textColor"), color.r, color.g, color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTextureID);
    glBindVertexArray(VAO);

    for (char c : text) {
        const Character& ch = Characters.at(c);

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 1.0f));

        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &model[0][0]);
        glUniform2f(prog->getUniform("uvOffset"), ch.UVOffset.x, ch.UVOffset.y);
        glUniform2f(prog->getUniform("uvSize"), ch.UVSize.x, ch.UVSize.y);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

