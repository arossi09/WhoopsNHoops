#pragma once
#include "Program.h"
#include "MatrixStack.h"
#include "AABB.h"
#include "Drone.h"

class Entity{
    public: 
        virtual ~Entity() {};
        virtual void update(float dt, Drone &drone) = 0;
        virtual std::shared_ptr<AABB> getAABB() = 0;
        virtual void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model)= 0;
};
