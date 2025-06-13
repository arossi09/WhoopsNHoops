#include <iostream>

#include "Lipo.h"

Lipo::Lipo(glm::vec3 pos, const std::string resourceDirectory){

    //create shape
    std::vector<tinyobj::shape_t> TOshapes;
    std::vector<tinyobj::material_t> objMaterials;
    //load in the mesh and make the shape(s)
    std::string errStr;
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/1slipo.obj").c_str());
    if (!rc) {
        std::cerr << errStr << std::endl;
    } else {
        shape = std::make_shared<Shape>();
        shape->createShape(TOshapes[0]);
        shape->measure();
        shape->init();
    }
    //create AABB
    auto lipo_AABB= std::make_shared<AABB>(shape->min, shape->max);
}

void Lipo::draw(std::shared_ptr<Program> prog){
    shape->draw(prog);
}

void Lipo::update(float dt){
    //need to implement
    return;
}

std::shared_ptr<AABB> Lipo::getAABB(){
    return lipo_AABB;
}

