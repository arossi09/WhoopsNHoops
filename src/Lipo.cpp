#include <iostream>

#include "Lipo.h"
#include "Drone.h"
#include "Program.h"

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

    //initlize the background prog
    shadowProg = std::make_shared<Program>();
    shadowProg->setVerbose(true);
    shadowProg->setShaderNames(resourceDirectory + "/lipo_shadow_vert.glsl", resourceDirectory + "/lipo_shadow_frag.glsl");
    shadowProg->init();
    shadowProg->addUniform("M");
    shadowProg->addUniform("V");
    shadowProg->addUniform("P");
    shadowProg->addAttribute("vertPos");
    shadowProg->addAttribute("vertNor");
}

//we need this to draw and transform the AABB
void Lipo::draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model){
    lipo_AABB->transform(Model->topMatrix());
    if(render){
        shape->draw(prog);
    }
}

void Lipo::draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> Model,
        std::shared_ptr<MatrixStack> View, std::shared_ptr<MatrixStack> Project){

    if(render){
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);
    prog->unbind();

    //draw background the prog
    shadowProg->bind();
    Model->scale(glm::vec3(1.1, 1.1, 1.02));
    glUniformMatrix4fv(shadowProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
    glUniformMatrix4fv(shadowProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniformMatrix4fv(shadowProg->getUniform("P"), 1, GL_FALSE, value_ptr(Project->topMatrix()));
    //need V & P
    shape->draw(shadowProg);
    shadowProg->unbind();

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    prog->bind();

    lipo_AABB->transform(Model->topMatrix());
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

