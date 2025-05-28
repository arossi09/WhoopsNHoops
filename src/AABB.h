#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Program;

class AABB{

    public:
        glm::vec3 min;
        glm::vec3 max;
        glm::vec3 originalMin;
        glm::vec3 originalMax;



        AABB(const glm::vec3 &min, const glm::vec3 &max);

        glm::vec3 getCenter() const;

        glm::vec3 getSize() const;

        bool intersects(const AABB& other) const;

        void transform(const glm::mat4& model);

        AABB transformed(const glm::mat4& model) const;

        std::vector<glm::vec3> getCorners() const;


        std::vector<glm::vec3> getOriginalCorners() const;

        void draw(const std::shared_ptr<Program> prog);

        void init();

    private:
       unsigned int vaoID = 0; 
       unsigned int posBufID = 0;
       unsigned int norBufID = 0;
       unsigned int eleBufID= 0;
       std::vector <glm::vec3> corners;
       std::vector <int> indices;

};
