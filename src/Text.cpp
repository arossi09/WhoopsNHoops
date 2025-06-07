
#include "Text.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <map>
unsigned int Text::VAO = 0;
unsigned int Text::VBO = 0;

void Text::load_characters(std::map<char, Character> &Characters){

   FT_Face face;

   FT_Library ft;

   if(FT_Init_FreeType(&ft)){
        std::cout << "ERROR::FREETYPE: Could not init Freetype library" <<std::endl;
        return;
   }

   if(FT_New_Face(ft, "../resources/PxPlus_IBM_VGA_8x16.ttf", 0, &face)){
        std::cout << "ERROR::FREETYPE: Could not load face" << std::endl;
        return;
   }

   FT_Set_Pixel_Sizes(face, 0, 48);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

   for(unsigned char c = 0; c < 128; c++){
        

       if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
       }

       //generate textures
       unsigned int texture;
       glGenTextures(1, &texture);
       glBindTexture(GL_TEXTURE_2D, texture);
       glTexImage2D(
               GL_TEXTURE_2D,
               0,
               GL_R8,
               face->glyph->bitmap.width,
               face->glyph->bitmap.rows,
               0,
               GL_RED,
               GL_UNSIGNED_BYTE,
               face->glyph->bitmap.buffer
               );

      //set Texture options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      //Character storage
      Character character = {
          texture,
          glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
          static_cast<unsigned int>(face->glyph->advance.x)
      };

      Characters.insert(std::pair<char, Character>(c, character));

   }
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glBindVertexArray(VAO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

   FT_Done_Face(face);
   FT_Done_FreeType(ft);
}

void Text::RenderText(std::shared_ptr<Program> prog, std::string text, float x, float y, float scale, glm::vec3 color, std::map<char, Character> Characters){


    glUniform3f(prog->getUniform("textColor"), color.r, color.g, color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;

    for(c = text.begin(); c != text.end(); c++){
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        //update VBO
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
}

