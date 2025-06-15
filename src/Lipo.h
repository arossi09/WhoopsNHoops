#pragma once
#include "Entity.h"
#include "Shape.h"
#include "Drone.h"
#include "AABB.h"



class Lipo : public Entity {

    public: 
        std::shared_ptr<AABB> lipo_AABB; 
        std::shared_ptr<Shape> shape;
        bool render = true;

        Lipo(glm::vec3 pos, const std::string resourceDirectory);
        void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model) override;
        void update(float dt, Drone &drone) override;
        std::shared_ptr<AABB> getAABB() override;
        //need to add function to call once i detect collision that takes
        //and alters drone state
    private:
        void chargeBattery(Drone drone);
};
