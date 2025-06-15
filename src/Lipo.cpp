#include <iostream>

#include "Lipo.h"
#include "Drone.h"

//need to make it so that timer resets lipo

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
    lipo_AABB = std::make_shared<AABB>(shape->min, shape->max);
}

//we need this to draw and transform the AABB
void Lipo::draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model){
    lipo_AABB->transform(Model->topMatrix());
    if(render){
        shape->draw(prog);
    }
}

void Lipo::update(float dt, Drone &drone){
    //charge drone battery;
    drone.battery += 25.0f;
    //disapear
    render = false;

    lipo_AABB->setCollide(false);
    
    //start timer
    return;
}

void Lipo::chargeBattery(Drone drone){
    drone.battery = 100.0f;
}

std::shared_ptr<AABB> Lipo::getAABB(){
    return lipo_AABB;
}

