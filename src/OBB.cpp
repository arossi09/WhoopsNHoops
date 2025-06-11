#include "OBB.h"
#include "GLSL.h"
#include "AABB.h"
#include "Program.h"


OBB::OBB(glm::vec3 &center, glm::vec3 &halfWidths, glm::mat3 orientation) :
    center(center), halfWidths(halfWidths), orientation(orientation){};

//this does not work too well
bool OBB::intersects(const OBB &other) const {
   const float EPSILON = 1e-6;
   glm::mat3 R, AbsR;

   //we need to compute the translation matrix to convert the other OBB
   //in this cordinates frame
   for(int i = 0; i < 3; i++)
       for(int j = 0; j < 3; j++)
           R[i][j] =glm::dot(orientation[i], other.orientation[i]);

   //we need to compute the translation vector
   glm::vec3 t = other.center - center;
   t =glm::vec3(glm::dot(t, orientation[0]), glm::dot(t, orientation[1]), glm::dot(t, orientation[2]));

   for(int i = 0; i < 3; i++)
       for(int j = 0; j < 3; j++)
           AbsR[i][j] = std::abs(R[i][j]) + EPSILON;

    for(int i = 0; i < 3; i++){
        float ra = halfWidths[i];
        float rb = other.halfWidths[0] * AbsR[i][0] + other.halfWidths[1] * AbsR[i][1] + other.halfWidths[2] * AbsR[i][2];
        if (std::abs(t[i]) > ra + rb) return false;
    }

    for (int i = 0; i < 3; i++) {
        float ra = halfWidths[0] * AbsR[0][i] + halfWidths[1] * AbsR[1][i] + halfWidths[2] * AbsR[2][i];
        float rb = other.halfWidths[i];
        if (std::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float ra = halfWidths[(i + 1) % 3] * AbsR[(i + 2) % 3][j] +
                       halfWidths[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
            float rb = other.halfWidths[(j + 1) % 3] * AbsR[i][(j + 2) % 3] +
                       other.halfWidths[(j + 2) % 3] * AbsR[i][(j + 1) % 3];
            float tProj = std::abs(t[(i + 2) % 3] * R[(i + 1) % 3][j] - t[(i + 1) % 3] * R[(i + 2) % 3][j]);
            if (tProj > ra + rb) return false;
        }
    }
    return true;

}

OBB OBB::transformed(const glm::mat4 model) const{
    glm::vec3 newCenter = glm::vec3(model * glm::vec4(center, 1.0f));

    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(model[0]));
    scale.y = glm::length(glm::vec3(model[1]));
    scale.z = glm::length(glm::vec3(model[2]));

    glm::mat3 rotation;
    rotation[0] = glm::normalize(glm::vec3(model[0]));
    rotation[1] = glm::normalize(glm::vec3(model[1]));
    rotation[2] = glm::normalize(glm::vec3(model[2]));

    glm::mat3 newOrientation = rotation ;

    glm::vec3 newHalfWidths = halfWidths * scale;

    return OBB(newCenter, newHalfWidths, newOrientation);
} 

bool OBB::intersects(const AABB &other) const {
    glm::vec3 center = (other.originalMin + other.originalMax) * 0.5f;
    glm::vec3 halfWidths = (other.originalMax - other.originalMin) * 0.5f;
    glm::mat3 orientation = glm::mat3(1.0f); // AABB = axis-aligned

    OBB otherOBB(center, halfWidths, orientation);
    return intersects(otherOBB);
}

OBB OBB::fromAABB(const AABB& box, const glm::mat4& model) const{
    glm::vec3 center = (box.originalMin + box.originalMax) * 0.5f;
    glm::vec3 halfWidths = (box.originalMax - box.originalMin) * 0.5f;
    glm::mat3 orientation = glm::mat3(1.0f); // initially identity
    return OBB(center, halfWidths, orientation).transformed(model);
}

AABB OBB::toAABB() const{
    glm::vec3 axes[3] = {
        orientation[0] * halfWidths.x,
        orientation[1] * halfWidths.y,
        orientation[2] * halfWidths.z
    };

    glm::vec3 corners[8];
    int idx = 0;
    for (int dx = -1; dx <= 1; dx += 2)
        for (int dy = -1; dy <= 1; dy += 2)
            for (int dz = -1; dz <= 1; dz += 2)
                corners[idx++] = center +
                    (float)dx * axes[0] +
                    (float)dy * axes[1] +
                    (float)dz * axes[2];

    glm::vec3 min = corners[0];
    glm::vec3 max = corners[0];
    for (int i = 1; i < 8; i++) {
        min = glm::min(min, corners[i]);
        max = glm::max(max, corners[i]);
    }
    return AABB(min, max);
}

glm::vec3 OBB::getCenter() const{
    return center;
}


glm::vec3 OBB::getSize() const{
    return halfWidths * glm::vec3(2.0f);
}

std::vector<glm::vec3> OBB::getCorners() const {
    std::vector<glm::vec3> corners;
    for (int x = -1; x <= 1; x += 2)
        for (int y = -1; y <= 1; y += 2)
            for (int z = -1; z <= 1; z += 2)
                corners.push_back(center + orientation * (halfWidths * glm::vec3(x, y, z)));
    return corners;
}


void OBB::initAxes(){
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);
}

void OBB::drawAxes(const std::shared_ptr<Program>& prog) const {
 glm::vec3 origin = center;
    float len = 1.0f;

    glm::vec3 x = orientation[0] * len;
    glm::vec3 y = orientation[1] * len;
    glm::vec3 z = orientation[2] * len;

    std::vector<glm::vec3> lines = {
        origin, origin + x, // X axis
        origin, origin + y, // Y axis
        origin, origin + z  // Z axis
    };

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(glm::vec3), lines.data(), GL_DYNAMIC_DRAW);

    int h_pos = prog->getAttribute("vertPos");
    glEnableVertexAttribArray(h_pos);
    glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_LINES, 0, 6);

    glDisableVertexAttribArray(h_pos);
    glBindVertexArray(0);
}
