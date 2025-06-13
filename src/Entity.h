#pragma once
#include "Program.h"
#include "AABB.h"

class Entity{
    public: 
        virtual ~Entity() {};
        virtual void update(float dt) = 0;
        virtual std::shared_ptr<AABB> getAABB() = 0;
        virtual void draw(std::shared_ptr<Program> prog) = 0;
};
