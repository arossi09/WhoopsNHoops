#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Program;
class AABB;

class OBB{



    public:
        glm::vec3 center;
        glm::vec3 halfWidths;
        glm::mat3 orientation;


        OBB(glm::vec3 &center, glm::vec3 &halfWidths, glm::mat3 orientation);

        
        OBB transformed(const glm::mat4 model) const;
        bool intersects(const OBB &other) const;
        bool intersects(const AABB &other) const;
        glm::vec3 getCenter() const;
        glm::vec3 getSize() const;
        void drawAxes(const std::shared_ptr<Program>& prog) const;
        void initAxes();
        AABB toAABB() const;


    private:
        std::vector<unsigned int> indices = {
            0, 1, 1, 3, 3, 2, 2, 0, // bottom face
            4, 5, 5, 7, 7, 6, 6, 4, // top face>
            0, 4, 1, 5, 2, 6, 3, 7  // vertical lines
        };
        OBB fromAABB(const AABB& box, const glm::mat4& model) const;
        std::vector<glm::vec3> getCorners() const;
        unsigned int axisVAO = 0, axisVBO = 0;


};

