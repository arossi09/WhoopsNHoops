
#include <memory>
#include "AABB.h"
#include "GLSL.h"
#include "Program.h"
#include "OBB.h"
#include "Drone.h"


AABB::AABB(const glm::vec3 &min, const glm::vec3 &max) : min(min), max(max), 
                        originalMin(min), originalMax(max){};

/*
AABB::AABB(const AABB& other)
    : min(other.min), max(other.max),
      originalMin(other.originalMin), originalMax(other.originalMax),
      corners(other.corners), indices(other.indices)
{}
*/

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

    glm::vec3 newMin = glm::vec3(model *glm::vec4(corners[0], 1.0f));
    glm::vec3 newMax = newMin;

    for(int i = 0; i < 8; i++){
        glm::vec3 pt = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        newMin = glm::min(newMin, pt);
        newMax = glm::max(newMax, pt);
    }

    min = newMin;
    max = newMax;
    //update new corners for draw
    corners = getCorners();


    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * 8, corners.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


AABB AABB::transformed(const glm::mat4& model) const {
    std::vector<glm::vec3> corners = getOriginalCorners();

    glm::vec3 newMin = glm::vec3(model * glm::vec4(corners[0], 1.0f));
    glm::vec3 newMax = newMin;

    for (int i = 0; i < 8; i++) {
        glm::vec3 pt = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        newMin = glm::min(newMin, pt);
        newMax = glm::max(newMax, pt);
    }

    return AABB(newMin, newMax);
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


std::vector<glm::vec3> AABB::getCorners() const{
    return {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {max.x, max.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };

}

std::shared_ptr<AABB> AABB::cloneTransformed(const glm::mat4& model) const {
    // copy base box
    auto newBox = std::make_shared<AABB>(*this); 
    // update min/max and buffer
    newBox->vaoID = this->vaoID;
    newBox->posBufID = this->posBufID;
    newBox->eleBufID = this->eleBufID;
    newBox->transform(model); 
    return newBox;
}

OBB AABB::toOBB() const {
    glm::vec3 center = (originalMin + originalMax) * 0.5f;
    glm::vec3 halfWidths = (originalMax - originalMin) * 0.5f;
    glm::mat3 orientation = glm::mat3(1.0f); // axis-aligned
    return OBB(center, halfWidths, orientation);
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
        0, 1, 1, 2, 2, 3, 3, 0, // bottom square
        4, 5, 5, 6, 6, 7, 7, 4, // top square
        0, 4, 1, 5, 2, 6, 3, 7  // vertical edges
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
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

    GLSL::disableVertexAttribArray(h_pos);
    glBindVertexArray(0);
}
