#include "AABB.h"

#include "GLSL.h"
#include "Program.h"


AABB::AABB(const glm::vec3 &min, const glm::vec3 &max) : min(min), max(max), 
                        originalMin(min), originalMax(max){};


glm::vec3 AABB::getCenter() const{
    return(min + max) * 0.5f;
} 

glm::vec3 AABB::getSize() const{
    return max - min;
}

bool AABB::intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y) &&
        (min.z <= other.max.z && max.z >= other.min.z);
}

void AABB::transform(const glm::mat4& model){
    std::vector<glm::vec3> corners = getOriginalCorners();
    std::vector<glm::vec3> transformed;
    transformed.reserve(8);

    glm::vec3 newMin = glm::vec3(model *glm::vec4(corners[0], 1.0f));
    glm::vec3 newMax = newMin;
    transformed.push_back(newMin);


    for(int i = 1; i < 8; i++){
        glm::vec3 pt = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        transformed.push_back(pt); 
        newMin = glm::min(newMin, pt);
        newMax = glm::max(newMax, pt);
    }

    min = newMin;
    max = newMax;

    /*
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * 8, transformed.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */

}


std::vector<glm::vec3> AABB::getOriginalCorners() const {
    return {
        {originalMin.x, originalMin.y, originalMin.z},
        {originalMax.x, originalMin.y, originalMin.z},
        {originalMax.x, originalMax.y, originalMin.z},
        {originalMin.x, originalMax.y, originalMin.z},
        {originalMin.x, originalMin.y, originalMax.z},
        {originalMax.x, originalMin.y, originalMax.z},
        {originalMax.x, originalMax.y, originalMax.z},
        {originalMin.x, originalMax.y, originalMax.z}
    };
}

void AABB::init(){
    corners = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, max.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };

    indices = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        0, 4, 5,  5, 1, 0,
        3, 2, 6,  6, 7, 3,
        0, 3, 7,  7, 4, 0,
        1, 5, 6,  6, 2, 1
    };

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &posBufID);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * corners.size(), corners.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &eleBufID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}



void AABB::draw(const std::shared_ptr<Program> prog){
    glBindVertexArray(vaoID);

    int h_pos = prog->getAttribute("vertPos");
    GLSL::enableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    GLSL::disableVertexAttribArray(h_pos);
    glBindVertexArray(0);
}
