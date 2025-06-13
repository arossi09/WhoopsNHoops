#pragma once
#include "Entity.h"
#include "Shape.h"
#include "AABB.h"



class Lipo : public Entity {

    public: 
        std::shared_ptr<AABB> lipo_AABB; 
        std::shared_ptr<Shape> shape;

        Lipo(glm::vec3 pos, const std::string resourceDirectory);
        void draw(std::shared_ptr<Program> prog) override;
        void update(float dt) override;
        std::shared_ptr<AABB> getAABB() override;
        //need to add function to call once i detect collision that takes
        //and alters drone state
};
