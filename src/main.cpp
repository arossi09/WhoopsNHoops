/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CSC 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */




//I could make a shape array to load each shape and then have enums
//for the index with the name being the index for that object
//TODO 
//make it so the obstacles are cell shaded solid colors
//make it so that there is function to draw silhoutes via cpu with 
//Text generation using a namespace
//look further into quaternions and how they work

#include <iostream>
#include <glad/glad.h>
#include <freetype2/ft2build.h> // might cause issues

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include FT_FREETYPE_H

#define PI 3.14

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program - use this one for Blinn-Phong
	std::shared_ptr<Program> prog;

	std::shared_ptr<Program> cellProg;

	std::shared_ptr<Program> silhoutteProg;

	//Our shader program for textures
	std::shared_ptr<Program> texProg;

	//our geometry
	shared_ptr<Shape> sphere;

	shared_ptr<Shape> cylinder1;

	shared_ptr<Shape> cylinder2;

	shared_ptr<Shape> ground;

	shared_ptr<Shape> house;

	shared_ptr<Shape> cementwall;

	shared_ptr<Shape> storageunit;

	shared_ptr<Shape> guardrail;

	shared_ptr<Shape> pallet;

	shared_ptr<Shape> metalfence;

	shared_ptr<Shape> walllong;

	shared_ptr<Shape> tiledwall;

	shared_ptr<Shape> telephone_pole;

	shared_ptr<Shape> bunny;

	shared_ptr<Shape> crate;

	shared_ptr<Shape> scaffolding;

	shared_ptr<Shape> wire;

	shared_ptr<Shape> stair_build;
	//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;


	//the image to use as a texture (ground)
	shared_ptr<Texture> texture0;

	shared_ptr<Texture> texture1;

	shared_ptr<Texture> texture2;

	shared_ptr<Texture> texture3;

	shared_ptr<Texture> texture4;

	shared_ptr<Texture> texture5;

	shared_ptr<Texture> texture6;

    shared_ptr<Texture> texture7;
    
    shared_ptr<Texture> texture8;

    shared_ptr<Texture> texture9;

    shared_ptr<Texture> texture10;

    shared_ptr<Texture> texture11;

    shared_ptr<Texture> texture12;

    shared_ptr<Texture> texture13;

    shared_ptr<Texture> texture14;

    struct Material{
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };

    //drone struct with attributes and update
    struct Drone {
       vec3 position = vec3(0.0f); 
       quat orientation = quat(1.0f, 0.0f, 0.0f, 0.0f);
       vec3 velocity = vec3(0.0f);
       vec3 acceleration = vec3(0.0f);
       float mass = 250.0f;
       float throttle = 0.0;
       float camera_title_angle = 25;

       //calculate drone physics
       void updatePosition(float dt){
            vec3 up = orientation * vec3(0, 1, 0);
            vec3 thrust = up * (throttle * 28000.0f); //Max thrust in N
          
            //prob have to fix this by balancing mass and thrust instead
            vec3 gravity = vec3(0, -45.0f, 0);
            vec3 netForce = thrust + (gravity * mass);
            acceleration = netForce/mass;

            //calculate the position through acceleration & velocity
            velocity += acceleration * dt;
            velocity *= .99f;
            position += velocity * dt;
       }

       void updateOrientation(float rollDelta, float pitchDelta, float yawDelta){
           // Create quaternions around local axes (apply roll -> pitch ->  yaw)
           glm::quat qRoll  = glm::angleAxis(rollDelta,  glm::vec3(0, 0, 1)); // local Z
           glm::quat qPitch = glm::angleAxis(pitchDelta, glm::vec3(1, 0, 0)); // local X
           glm::quat qYaw   = glm::angleAxis(yawDelta,   glm::vec3(0, 1, 0)); // local Y
           orientation = orientation * qYaw * qPitch * qRoll;
           orientation = glm::normalize(orientation);
       }
    };


    Material Material1 = {
        vec3(0.046f, 0.046f, 0.045f),  // ambient (dim gray)
        vec3(0.46f, 0.46f, 0.45f),     // diffuse (light gray)
        vec3(0.45f, 0.43f, 0.45f),     // specular (soft silver)
        120.0f                         // shininess (sharp highlight)
    };

    Material Material2 = {
        vec3(0.1f, 0.05f, 0.05f),      // ambient (reddish low light)
        vec3(0.7f, 0.2f, 0.2f),        // diffuse (red tone)
        vec3(0.8f, 0.3f, 0.3f),        // specular (shiny red highlight)
        64.0f                          // shininess (less sharp)
    };

    Material Material3 = {
        vec3(0.02f, 0.05f, 0.1f),      // ambient (cool blue)
        vec3(0.2f, 0.4f, 0.7f),        // diffuse (blue tone)
        vec3(0.4f, 0.6f, 0.9f),        // specular (strong blue highlight)
        16.0f                          // shininess (soft and wide highlight)
    };





        //stack for shapes
    vector <shared_ptr<Shape>> shapes;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;
    vec3 gMax;


    Drone drone;
    float phi = 0.0f;
    float theta = PI/2;
    float roll = 0;

    //gamepad 
    float yawDelta = 0;
    float pitchDelta = 0;
    float rollDelta = 0;

	//global data (larger program should be encapsulated)
	float gRot = 0;
	float gCamH = 0;
    float sensitivity = .1 ;
    float radius = 1.0f;
	//animation data
	float lightTrans = 0;
	float gTrans = -3;
	float sTheta = 0;
    float cTheta = 0;
	float eTheta = 0;
	float hTheta = 0;
    int MatToggle = 0;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		//update global camera rotate
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			yawDelta = 0.5;
		}

		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			yawDelta = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			yawDelta = -.5f;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			yawDelta = 0;
		}

		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			drone.throttle= 1;
		}

		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			drone.throttle= .45;
		}

		//update camera height
		if (key == GLFW_KEY_S && action == GLFW_PRESS){
		    drone.throttle = 0;	
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS){
			gCamH  -= 1.25;
		}


		if (key == GLFW_KEY_M && action == GLFW_PRESS){
            MatToggle = !MatToggle;
		}

		if (key == GLFW_KEY_Q && action == GLFW_PRESS){
			lightTrans += 1.25;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS){
			lightTrans -= 1.25;
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

        drone.updateOrientation(rollDelta, pitchDelta, yawDelta);
	} 

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;


        glfwGetCursorPos(window, &posX, &posY);
        cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
	}

    //gather the deltaX and deltaY on scroll and change the phi and theta
    //based off the sensitivity
    void scrollCallback(GLFWwindow *window, double deltaX, double deltaY){
       phi -= deltaY * sensitivity;
       theta += deltaX * sensitivity;
       if(phi > 80) phi = 80;
       if(phi < -80) phi = -80;
    }


    //gather the controller inputs on callback
    void gamepadInputCallback(float leftX, float leftY, float rightX, float rightY){
        /*
        cout << "leftX" << leftX << endl;
        cout << "leftY" << leftY << endl;
        cout << "rightX" << rightX << endl;
        cout << "rightY" << rightY << endl;

        //leftX yaw
        theta -= leftX * sensitivity*6;
        //pitch
        phi += rightY * sensitivity*6;
        //right X rotate camera
        cRot += rightX;
        //leftY throttle
        //
        //*/
        
        //turn controller axie location into drone movement data
        yawDelta   = -leftX * .07;
        pitchDelta =  rightY * .07;
        rollDelta  =  rightX * .07;
        //clamp throttle [0, 1]
        drone.throttle = (leftY+1)/2;
        drone.updateOrientation(rollDelta, pitchDelta, yawDelta);
    }


    /*
     * Regular controls
    //with the phi and theta calculate the direction vectur and 
    void updateCamera(shared_ptr<MatrixStack> &view){
        vec3 direction;
        vec3 eye = vec3(0, 10, -30);
        eye.y += gCamH;
        direction.x = radius*cos(glm::radians(phi)) * cos(glm::radians(theta));
        direction.y = radius*sin(glm::radians(phi));
        direction.z = radius*cos(glm::radians(phi)) * cos((PI/2) -radians(theta));
        direction = glm::normalize(direction);

        //eye + direction for look at because we need look at relative 
        //to where camera is 
        view->lookAt(eye, eye+direction, vec3(0, 1, 0));
    }
    */


    float calculateDeltaTime(){
        using clock = chrono::high_resolution_clock;
        static auto lastTime = clock::now();

        auto currentTime = clock::now();
        chrono::duration<float> delta = currentTime - lastTime;
        lastTime = currentTime;

        return delta.count();
    }

    //update camera location and orrientation based off drone
    void updateCamera(shared_ptr<MatrixStack> &view, 
            vec3 drone_position, quat drone_orientation, float drone_camera_angle){
       
        //rotate around x axis to pitch
        quat cameraPitch = angleAxis(radians(drone_camera_angle), vec3(1, 0, 0));

        quat cameraOrientation = drone_orientation * cameraPitch;

        vec3 eye = drone_position;
        vec3 forward = cameraOrientation* vec3(0.0f, 0.0f, -1.0f);
        vec3 up      = cameraOrientation* vec3(0.0f, 1.0f,  0.0f);
        //to where camera is 
        view->lookAt(eye, eye+forward, up);
    }

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
        sensitivity = 180.0f / height;
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

        //this is for phongg shading flat
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
        prog->addUniform("material.ambient");
        prog->addUniform("material.diffuse");
        prog->addUniform("material.specular");
        prog->addUniform("material.shininess");
		prog->addUniform("lightPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");



        //cell shading
		cellProg = make_shared<Program>();
		cellProg->setVerbose(true);
		cellProg->setShaderNames(resourceDirectory + "/cell_vert.glsl", resourceDirectory + "/cell_frag.glsl");
		cellProg->init();
		cellProg->addUniform("P");
		cellProg->addUniform("V");
		cellProg->addUniform("M");
		cellProg->addUniform("lightPos");
		cellProg->addUniform("baseColor");
		cellProg->addAttribute("vertPos");
		cellProg->addAttribute("vertNor");




		// Initialize the GLSL program that we will use for texture mapping
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
        texProg->addUniform("lightDirection");
        texProg->addUniform("flip");
		texProg->addUniform("Texture0");
		texProg->addUniform("lightToggle");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

        silhoutteProg = make_shared<Program>();
        silhoutteProg->setVerbose(true);
        silhoutteProg->setShaderNames(resourceDirectory + "/silhoutte_vert.glsl", resourceDirectory + "/silhoutte_frag.glsl");
        silhoutteProg->init();
        silhoutteProg->addUniform("P");
        silhoutteProg->addUniform("V");
        silhoutteProg->addUniform("M");


		//read in a load the texture
		texture0 = make_shared<Texture>();
  		texture0->setFilename(resourceDirectory + "/house.png");
  		texture0->init();
  		texture0->setUnit(0);
  		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture0->setFiltering(GL_NEAREST, GL_NEAREST);



		texture1 = make_shared<Texture>();
  		texture1->setFilename(resourceDirectory + "/sky_28_2k.png");
  		texture1->init();
  		texture1->setUnit(1);
  		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture1->setFiltering(GL_NEAREST, GL_NEAREST);



		texture2 = make_shared<Texture>();
  		texture2->setFilename(resourceDirectory + "/ground.png");
  		texture2->init();
  		texture2->setUnit(2);
  		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture2->setFiltering(GL_NEAREST, GL_NEAREST);


		texture3 = make_shared<Texture>();
  		texture3->setFilename(resourceDirectory + "/wall.png");
  		texture3->init();
  		texture3->setUnit(3);
  		texture3->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture3->setFiltering(GL_NEAREST, GL_NEAREST);


		texture4 = make_shared<Texture>();
  		texture4->setFilename(resourceDirectory + "/concretewall.png");
  		texture4->init();
  		texture4->setUnit(3);
  		texture4->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture4->setFiltering(GL_NEAREST, GL_NEAREST);


		texture5 = make_shared<Texture>();
  		texture5->setFilename(resourceDirectory + "/water.png");
  		texture5->init();
  		texture5->setUnit(0);
  		texture5->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture5->setFiltering(GL_NEAREST, GL_NEAREST);


		texture6 = make_shared<Texture>();
  		texture6->setFilename(resourceDirectory + "/storageunit.png");
  		texture6->init();
  		texture6->setUnit(0);
  		texture6->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture6->setFiltering(GL_NEAREST, GL_NEAREST);



		texture7 = make_shared<Texture>();
  		texture7->setFilename(resourceDirectory + "/guardrail.png");
  		texture7->init();
  		texture7->setUnit(0);
  		texture7->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture7->setFiltering(GL_NEAREST, GL_NEAREST);


		texture8 = make_shared<Texture>();
  		texture8->setFilename(resourceDirectory + "/wood.png");
  		texture8->init();
  		texture8->setUnit(0);
  		texture8->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture8->setFiltering(GL_NEAREST, GL_NEAREST);



		texture9 = make_shared<Texture>();
  		texture9->setFilename(resourceDirectory + "/metalfence.png");
  		texture9->init();
  		texture9->setUnit(0);
  		texture9->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture9->setFiltering(GL_NEAREST, GL_NEAREST);

		texture10 = make_shared<Texture>();
  		texture10->setFilename(resourceDirectory + "/walllong.png");
  		texture10->init();
  		texture10->setUnit(0);
  		texture10->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture10->setFiltering(GL_NEAREST, GL_NEAREST);


		texture11 = make_shared<Texture>();
  		texture11->setFilename(resourceDirectory + "/tiledwall.png");
  		texture11->init();
  		texture11->setUnit(0);
  		texture11->setWrapModes(GL_REPEAT, GL_REPEAT);
        texture11->setFiltering(GL_NEAREST, GL_NEAREST);


		texture12 = make_shared<Texture>();
  		texture12->setFilename(resourceDirectory + "/scaffolding.png");
  		texture12->init();
  		texture12->setUnit(0);
  		texture12->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture12->setFiltering(GL_NEAREST, GL_NEAREST);




		texture13 = make_shared<Texture>();
  		texture13->setFilename(resourceDirectory + "/industrial_misc.png");
  		texture13->init();
  		texture13->setUnit(0);
  		texture13->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture13->setFiltering(GL_NEAREST, GL_NEAREST);



		texture14 = make_shared<Texture>();
  		texture14->setFilename(resourceDirectory + "/stair_building.png");
  		texture14->init();
  		texture14->setUnit(0);
  		texture14->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture14->setFiltering(GL_NEAREST, GL_NEAREST);


	}

	void initGeom(const std::string& resourceDirectory)
	{



 		vector<tinyobj::shape_t> TOshapesZ;
 		vector<tinyobj::material_t> objMaterialsZ;
 		string errStr;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapesZ, objMaterialsZ, errStr, (resourceDirectory + "/bunnyNoNorm.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			bunny= make_shared<Shape>();
			bunny->createShape(TOshapesZ[0]);
			bunny->measure();
			bunny->init();
		}

 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/sphereWTex.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			sphere = make_shared<Shape>();
			sphere->createShape(TOshapes[0]);
			sphere->measure();
			sphere->init();
		}

		// Initialize bunny mesh.
		vector<tinyobj::shape_t> TOshapesB;
 		vector<tinyobj::material_t> objMaterialsB;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesB, objMaterialsB, errStr, (resourceDirectory + "/cylinder1.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			cylinder1 = make_shared<Shape>();
			cylinder1->createShape(TOshapesB[0]);
			cylinder1->measure();
			cylinder1->init();
		}




		vector<tinyobj::shape_t> TOshapesC;
 		vector<tinyobj::material_t> objMaterialsC;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesC, objMaterialsB, errStr, (resourceDirectory + "/ground.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			ground= make_shared<Shape>();
			ground->createShape(TOshapesC[0]);
			ground->measure();
			ground->init();
		}



		vector<tinyobj::shape_t> TOshapesD;
 		vector<tinyobj::material_t> objMaterialsD;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesD, objMaterialsD, errStr, (resourceDirectory + "/house.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			house= make_shared<Shape>();
			house->createShape(TOshapesD[0]);
			house->measure();
			house->init();
		}


		vector<tinyobj::shape_t> TOshapesE;
 		vector<tinyobj::material_t> objMaterialsE;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesE, objMaterialsE, errStr, (resourceDirectory + "/cylinder2.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			cylinder2= make_shared<Shape>();
			cylinder2->createShape(TOshapesE[0]);
			cylinder2->measure();
			cylinder2->init();
		}



		vector<tinyobj::shape_t> TOshapesF;
 		vector<tinyobj::material_t> objMaterialsF;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesF, objMaterialsF, errStr, (resourceDirectory + "/cementwall.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			cementwall= make_shared<Shape>();
			cementwall->createShape(TOshapesF[0]);
			cementwall->measure();
			cementwall->init();
		}



		vector<tinyobj::shape_t> TOshapesG;
 		vector<tinyobj::material_t> objMaterialsG;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesG, objMaterialsG, errStr, (resourceDirectory + "/storageunit.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			storageunit = make_shared<Shape>();
		    storageunit->createShape(TOshapesG[0]);
			storageunit->measure();
			storageunit->init();
		}



		vector<tinyobj::shape_t> TOshapesH;
 		vector<tinyobj::material_t> objMaterialsH;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesH, objMaterialsH, errStr, (resourceDirectory + "/guardrail.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			guardrail= make_shared<Shape>();
		    guardrail->createShape(TOshapesH[0]);
			guardrail->measure();
			guardrail->init();
		}


		vector<tinyobj::shape_t> TOshapesI;
 		vector<tinyobj::material_t> objMaterialsI;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesI, objMaterialsI, errStr, (resourceDirectory + "/pallet.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			pallet= make_shared<Shape>();
		    pallet->createShape(TOshapesI[0]);
			pallet->measure();
			pallet->init();
		}


		vector<tinyobj::shape_t> TOshapesJ;
 		vector<tinyobj::material_t> objMaterialsJ;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesJ, objMaterialsJ, errStr, (resourceDirectory + "/metalfence.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			metalfence= make_shared<Shape>();
		    metalfence->createShape(TOshapesJ[0]);
			metalfence->measure();
			metalfence->init();
		}

		vector<tinyobj::shape_t> TOshapesK;
 		vector<tinyobj::material_t> objMaterialsK;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesK, objMaterialsK, errStr, (resourceDirectory + "/walllong.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			walllong= make_shared<Shape>();
		    walllong->createShape(TOshapesK[0]);
			walllong->measure();
			walllong->init();
		}



		vector<tinyobj::shape_t> TOshapesL;
 		vector<tinyobj::material_t> objMaterialsL;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesL, objMaterialsL, errStr, (resourceDirectory + "/tiledwall.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			tiledwall= make_shared<Shape>();
		    tiledwall->createShape(TOshapesL[0]);
			tiledwall->measure();
			tiledwall->init();
		}



		vector<tinyobj::shape_t> TOshapesM;
 		vector<tinyobj::material_t> objMaterialsM;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesM, objMaterialsM, errStr, (resourceDirectory + "/storagecrate.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			crate= make_shared<Shape>();
		    crate->createShape(TOshapesM[0]);
			crate->measure();
			crate->init();
		}


		vector<tinyobj::shape_t> TOshapesN;
 		vector<tinyobj::material_t> objMaterialsN;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesN, objMaterialsN, errStr, (resourceDirectory + "/telephone_pole.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			telephone_pole= make_shared<Shape>();
		    telephone_pole->createShape(TOshapesN[0]);
			telephone_pole->measure();
			telephone_pole->init();
		}

		vector<tinyobj::shape_t> TOshapesO;
 		vector<tinyobj::material_t> objMaterialsO;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesO, objMaterialsO, errStr, (resourceDirectory + "/scaffolding.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			scaffolding= make_shared<Shape>();
		    scaffolding->createShape(TOshapesO[0]);
			scaffolding->measure();
			scaffolding->init();
		}



		vector<tinyobj::shape_t> TOshapesP;
 		vector<tinyobj::material_t> objMaterialsP;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesP, objMaterialsP, errStr, (resourceDirectory + "/wire.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			wire= make_shared<Shape>();
		    wire->createShape(TOshapesP[0]);
			wire->measure();
			wire->init();
		}


		vector<tinyobj::shape_t> TOshapesQ;
 		vector<tinyobj::material_t> objMaterialsQ;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesQ, objMaterialsQ, errStr, (resourceDirectory + "/stair_building.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			stair_build= make_shared<Shape>();
		    stair_build->createShape(TOshapesQ[0]);
			stair_build->measure();
			stair_build->init();
		}

        //load in dummy.obj multi shape object
        vec3 minBounds = vec3(numeric_limits<float>::max());
        vec3 maxBounds = vec3(-numeric_limits<float>::max());

		vector<tinyobj::shape_t> TOshapes3;
 		rc = tinyobj::LoadObj(TOshapes3, objMaterials, errStr, (resourceDirectory + "/wharehouse.obj").c_str());

		if (!rc) {
			cerr << errStr << endl;
		} else {
            //loop through the shapes and calculate the total bounding box for the entire
            //model
            for(size_t i = 0; i < TOshapes3.size(); i ++){


                auto shape = make_shared<Shape>();
                shape->createShape(TOshapes3[i]);
                shape->measure();
                shape->init();

                //add each shape to a shape stack
                shapes.push_back(shape);

                //calculate min max from all

                minBounds.x = std::min(minBounds.x, shape->min.x);
                minBounds.y = std::min(minBounds.y, shape->min.y);
                minBounds.z = std::min(minBounds.z, shape->min.z);

                maxBounds.x = std::max(maxBounds.x, shape->max.x);
                maxBounds.y = std::max(maxBounds.y, shape->max.y);
                maxBounds.z = std::max(maxBounds.z, shape->max.z);
            }
		}

		//read out information stored in the shape about its size - something like this...
		//then do something with that information.....
		gMin = minBounds;
        gMax = maxBounds;


		//code to load in the ground plane (CPU defined data passed to GPU)
		initGround();
	}

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = 200;
		float g_groundY = -8;

  		// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY+sTheta*2, -g_groundSize,
			-g_groundSize, g_groundY+cTheta*2,  g_groundSize,
			g_groundSize, g_groundY+sTheta*2,  g_groundSize,
			g_groundSize, g_groundY+cTheta*2, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		static GLfloat GrndTex[] = {
      		0, 0, // back
      		0, 1,
      		1, 1,
      		1, 0 };

      	unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		//generate the ground VAO
      	glGenVertexArrays(1, &GroundVertexArrayID);
      	glBindVertexArray(GroundVertexArrayID);

      	g_GiboLen = 6;
      	glGenBuffers(1, &GrndBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndNorBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndTexBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

      	glGenBuffers(1, &GIndxBuffObj);
     	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
      }

      //code to draw the ground plane
     void drawGround(shared_ptr<Program> curS) {
     	curS->bind();
     	glBindVertexArray(GroundVertexArrayID);
        glUniform1i(curS->getUniform("lightToggle"), 0); 
     	texture5->bind(curS->getUniform("Texture0"));

		//draw the ground plane 
  		SetModel(vec3(0, -1, 0), 0, 0, 1, curS);
  		glEnableVertexAttribArray(0);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
  		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(1);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
  		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(2);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
  		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   		// draw!
  		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
  		glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

  		glDisableVertexAttribArray(0);
  		glDisableVertexAttribArray(1);
  		glDisableVertexAttribArray(2);
  		curS->unbind();
     }

     //helper function to pass material data to the GPU
	void SetMaterial(shared_ptr<Program> curS, int i) {

    	switch (i) {
    		case 0: //
    			glUniform3f(curS->getUniform("MatAmb"), 0.096, 0.046, 0.095);
    			glUniform3f(curS->getUniform("MatDif"), 0.96, 0.46, 0.95);
    			glUniform3f(curS->getUniform("MatSpec"), 0.45, 0.23, 0.45);
    			glUniform1f(curS->getUniform("MatShine"), 120.0);
    		break;
    		case 1: // 
    			glUniform3f(curS->getUniform("MatAmb"), 0.063, 0.038, 0.1);
    			glUniform3f(curS->getUniform("MatDif"), 0.63, 0.38, 1.0);
    			glUniform3f(curS->getUniform("MatSpec"), 0.3, 0.2, 0.5);
    			glUniform1f(curS->getUniform("MatShine"), 4.0);
    		break;
    		case 2: //
    			glUniform3f(curS->getUniform("MatAmb"), 0.004, 0.05, 0.09);
    			glUniform3f(curS->getUniform("MatDif"), 0.04, 0.5, 0.9);
    			glUniform3f(curS->getUniform("MatSpec"), 0.02, 0.25, 0.45);
    			glUniform1f(curS->getUniform("MatShine"), 27.9);
    		break;
  		}
	}

	/* helper function to set model trasnforms */
  	void SetModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
  		mat4 ctm = Trans*RotX*RotY*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   	}


    void resize_and_center(vec3 gMin, vec3 gMax, shared_ptr<MatrixStack> Model){
        float center_x = (gMax.x + gMin.x)/2;
        float center_y = (gMax.y + gMin.y)/2;
        float center_z = (gMax.z + gMin.z)/2;

        float largest_extent = std::max( std::max((gMax.x - gMin.x), (gMax.y - gMin.y)), (gMax.z - gMin.z));
        float scale = 2.0/largest_extent;
        Model->translate(vec3(-center_x, -center_y, -center_z));
        Model->scale(vec3(scale, scale, scale));
    }

    void set_material_uniforms(std::shared_ptr<Program> prog, const Material &mat){
        glUniform3fv(prog->getUniform("material.ambient"), 1, glm::value_ptr(mat.ambient));
        glUniform3fv(prog->getUniform("material.diffuse"), 1, glm::value_ptr(mat.diffuse));
        glUniform3fv(prog->getUniform("material.specular"), 1, glm::value_ptr(mat.specular));
        glUniform1f(prog->getUniform("material.shininess"), mat.shininess);
    }


	void render() {
		// Get current frame buffer size.
		int width, height;

		initGround();
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.5f, aspect, 0.01f, 400.0f);

		// View is global translation along negative z for now
		View->pushMatrix();
		View->loadIdentity();
        updateCamera(View, drone.position, drone.orientation, drone.camera_title_angle);
        

        //offload this to function

        //draw skybox
        texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(texProg->getUniform("lightDirection"), 1, -1, 1);
		glUniform1i(texProg->getUniform("flip"), 0); 
		glUniform1i(texProg->getUniform("lightToggle"), 0); 
        
        Model->pushMatrix();
            texture1->bind(texProg->getUniform("Texture0"));
            Model->scale(vec3(200, 200, 200));
            setModel(texProg, Model);
            sphere->draw(texProg);
        Model->popMatrix();
        texProg->unbind();

		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), 2.0+lightTrans, 2.0+lightTrans, 2.9+lightTrans);
        if(MatToggle){
            set_material_uniforms(prog, Material1);
        }else{
            set_material_uniforms(prog, Material2);
        }
   
        /*
        Model->pushMatrix();
            Model->translate(vec3(0, 5, 20));
            Model->rotate(-PI/2, vec3(0, 1, 0));
            Model->scale(vec3(200, 200, 200));
            setModel(prog, Model);
            bunny->draw(prog);
        Model->popMatrix();

        Model->pushMatrix();
            set_material_uniforms(prog, Material3);
            Model->translate(vec3(-30, -3, 20));
            Model->rotate(-PI/6, vec3(0, 1, 0));
            Model->scale(vec3(200, 200, 200));
            setModel(prog, Model);
            bunny->draw(prog);
        Model->popMatrix();
        */

        prog->unbind();
        cellProg->bind();
		glUniformMatrix4fv(cellProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(cellProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(cellProg->getUniform("lightPos"), 0+lightTrans, 20.0+lightTrans, 0+lightTrans);

        //cylinder1
        cellProg->unbind();

        //Main scene
        texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(texProg->getUniform("lightDirection"), 1, -1, 1); 
		glUniform1i(texProg->getUniform("flip"), 1); 
		glUniform1i(texProg->getUniform("lightToggle"), 1); 

        Model->pushMatrix();
            Model->translate(vec3(0, 2, 0));
            Model->scale(vec3(2, 2, 2));



            //cylinder  in front
            Model->pushMatrix();
                texture3->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-3.7, -.2f, 2));
                Model->rotate(-PI/6, vec3(0, 1, 0));//turn
                Model->rotate(PI/2.7, vec3(1, 0, 0));//rotate on side
                Model->rotate(PI/2, vec3(0, 0, 1));//flip over

                //Model->scale(vec3(1, ));
                resize_and_center(cylinder1->min, cylinder1->max, Model);
                setModel(texProg, Model);
                cylinder1->draw(texProg);
            Model->popMatrix();


            //cylinder in back
            Model->pushMatrix();
                texture3->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-6.5f, -.3f, 1.5));
                Model->rotate(PI/2.7, vec3(1, 0, 0));//rotate on side
                Model->rotate(PI/2, vec3(0, 0, 1));
                Model->scale(vec3(.7f,.7f,.7f));
                resize_and_center(cylinder1->min, cylinder1->max, Model);
                setModel(texProg, Model);
                cylinder1->draw(texProg);
            Model->popMatrix();


            //cylinder along wall
            Model->pushMatrix();
                texture3->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(2, -.2f, 3));
                Model->rotate(PI/2, vec3(0, 1, 0));//turn
                Model->rotate(PI/2.7, vec3(1, 0, 0));//rotate on side
                Model->rotate(PI/2, vec3(0, 0, 1));
                Model->scale(vec3(1, 3,1));
                resize_and_center(cylinder1->min, cylinder1->max, Model);
                setModel(texProg, Model);
                cylinder1->draw(texProg);
            Model->popMatrix();

            //ground
            Model->pushMatrix();
                Model->translate(vec3(0, -5, 5));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(40, 10, 40));
                texture2->bind(texProg->getUniform("Texture0"));
                resize_and_center(ground->min, ground->max, Model);
                setModel(texProg, Model);
                ground->draw(texProg);
            Model->popMatrix();


            Model->pushMatrix();
                Model->translate(vec3(0, -5, 60));
                Model->rotate(PI/2, vec3(0, 1, 0));
                Model->scale(vec3(40, 10, 40));
                texture2->bind(texProg->getUniform("Texture0"));
                resize_and_center(ground->min, ground->max, Model);
                setModel(texProg, Model);
                ground->draw(texProg);
            Model->popMatrix();


            //house to left
            Model->pushMatrix();
                texture0->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(13, 10, -2));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(8, 8, 8));
                resize_and_center(house->min, house->max, Model);
                setModel(texProg, Model);
                house->draw(texProg);
            Model->popMatrix();


            //cementwall
            Model->pushMatrix();
                texture4->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(4.2f, 1.2f, 0));
                Model->rotate(PI/2, vec3(0, 1, 0));
                Model->scale(vec3(7, 9, 7));
                resize_and_center(cementwall->min, cementwall->max, Model);
                setModel(texProg, Model);
                cementwall->draw(texProg);
                Model->pushMatrix();
                    Model->translate(vec3(-24, 0, 0));
                    Model->scale(vec3(1.5, 1, 1));
                    setModel(texProg, Model);
                    cementwall->draw(texProg);
                Model->popMatrix();
            Model->popMatrix();

            //storage unit
            Model->pushMatrix();
                texture6->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-2, 14.5f, 13));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(6, 7, 6));
                resize_and_center(storageunit->min, storageunit->max, Model);
                setModel(texProg, Model);
                storageunit->draw(texProg);
            Model->popMatrix();

            //guard rail middle left1
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(0, -.35f, -18));
                Model->rotate(-PI/2.1, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail->min, guardrail->max, Model);
                setModel(texProg, Model);
                guardrail->draw(texProg);

            Model->popMatrix();

            //guard rail middle right
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(12, -.35f, -18));
                Model->rotate(-PI/1.9, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail->min, guardrail->max, Model);
                setModel(texProg, Model);
                guardrail->draw(texProg);

            Model->popMatrix();

            //guard rail most right
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(28, -.35f, -18));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail->min, guardrail->max, Model);
                setModel(texProg, Model);
                guardrail->draw(texProg);
            Model->popMatrix();


            //guard rail most left
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-30, -.35f, -18));
                Model->rotate(-PI/2.1, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail->min, guardrail->max, Model);
                setModel(texProg, Model);
                guardrail->draw(texProg);
            Model->popMatrix();


            //guard rail left
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-18, -.35f, -18));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail->min, guardrail->max, Model);
                setModel(texProg, Model);
                guardrail->draw(texProg);
            Model->popMatrix();

            //pallet laying on top
            Model->pushMatrix();
                texture8->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(2.5f, .5f, -4));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->rotate(PI/3, vec3(1, 0, 0));
                Model->scale(vec3(2, 2.5f, 2));
                resize_and_center(pallet->min, pallet->max, Model);
                setModel(texProg, Model);
                pallet->draw(texProg);
            Model->popMatrix();

            //pallet on floor
            Model->pushMatrix();
                texture8->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-20, -.8f, -3));
                Model->rotate(PI/4, vec3(0, 1, 0));
                Model->scale(vec3(2, 3, 2));
                resize_and_center(pallet->min, pallet->max, Model);
                setModel(texProg, Model);
                pallet->draw(texProg);
            Model->popMatrix();

            //walls left side
            Model->pushMatrix();
                Model->translate(vec3(1, 0, -2));
                //metalfence right side most left
                Model->pushMatrix();
                    texture9->bind(texProg->getUniform("Texture0"));
                    Model->translate(vec3(38, 2, -5));
                    Model->rotate(-PI/2, vec3(0, 1, 0));
                    Model->scale(vec3(3, 3, 3));
                    resize_and_center(metalfence->min, metalfence->max, Model);
                    setModel(texProg, Model);
                    metalfence->draw(texProg);
                    Model->pushMatrix();
                        texture9->bind(texProg->getUniform("Texture0"));
                        Model->translate(vec3(8.5f, 0, 0));
                        setModel(texProg, Model);
                        metalfence->draw(texProg);
                    Model->popMatrix();
                Model->popMatrix();


                Model->pushMatrix();
                    texture9->bind(texProg->getUniform("Texture0"));
                    Model->translate(vec3(38, 2, 14));
                    Model->rotate(-PI/2, vec3(0, 1, 0));
                    Model->scale(vec3(3, 3, 3));
                    resize_and_center(metalfence->min, metalfence->max, Model);
                    setModel(texProg, Model);
                    metalfence->draw(texProg);
                    Model->pushMatrix();
                        texture9->bind(texProg->getUniform("Texture0"));
                        Model->translate(vec3(8.5f, 0, 0));
                        setModel(texProg, Model);
                        metalfence->draw(texProg);
                    Model->popMatrix();
                Model->popMatrix();





                //walllong right most left
                Model->pushMatrix();
                    texture10->bind(texProg->getUniform("Texture0"));
                    Model->translate(vec3(37, 2, -10.75));
                    Model->rotate(-PI/3, vec3(0, 1, 0));
                    Model->scale(vec3(3.5f, 3.5f, 3.5f));
                    resize_and_center(walllong->min, walllong->max, Model);
                    setModel(texProg, Model);
                    walllong->draw(texProg);
                Model->popMatrix();

                Model->pushMatrix();
                    texture10->bind(texProg->getUniform("Texture0"));
                    Model->translate(vec3(38.5f, 2, 7.5f));
                    Model->rotate(PI/2, vec3(0, 1, 0));
                    Model->scale(vec3(3.5f, 3.5f, 3.5f));
                    resize_and_center(walllong->min, walllong->max, Model);
                    setModel(texProg, Model);
                    walllong->draw(texProg);
                Model->popMatrix();

            Model->popMatrix();


            //walls right side
            Model->pushMatrix();
                Model->translate(vec3(1, 0, -2));
                //metalfence right side most left
                Model->pushMatrix();
                    texture9->bind(texProg->getUniform("Texture0"));
                    Model->translate(vec3(-40, 2, 1));
                    Model->rotate(PI/2, vec3(0, 1, 0));
                    Model->scale(vec3(3, 3, 3));
                    resize_and_center(metalfence->min, metalfence->max, Model);
                    setModel(texProg, Model);
                    metalfence->draw(texProg);
                    Model->pushMatrix();
                        texture9->bind(texProg->getUniform("Texture0"));
                        Model->translate(vec3(8.5f, 0, 0));
                        setModel(texProg, Model);
                        metalfence->draw(texProg);
                    Model->popMatrix();
                Model->popMatrix();


                Model->pushMatrix();
                    texture9->bind(texProg->getUniform("Texture0"));
                    Model->translate(vec3(-40, 2, 14));
                    Model->rotate(PI/2, vec3(0, 1, 0));
                    Model->scale(vec3(3, 3, 3));
                    resize_and_center(metalfence->min, metalfence->max, Model);
                    setModel(texProg, Model);
                    metalfence->draw(texProg);
                    Model->pushMatrix();
                        texture9->bind(texProg->getUniform("Texture0"));
                        Model->translate(vec3(-8.5f, 0, 0));
                        setModel(texProg, Model);
                        metalfence->draw(texProg);
                    Model->popMatrix();
                Model->popMatrix();





                //walllong right most left
                Model->pushMatrix();
                    //cement wall next to light
                    texture10->bind(texProg->getUniform("Texture0"));
                    Model->translate(vec3(-39, 2, -10.75));
                    Model->rotate(PI/3, vec3(0, 1, 0));
                    Model->scale(vec3(3.5f, 3.5f, 3.5f));
                    resize_and_center(walllong->min, walllong->max, Model);
                    setModel(texProg, Model);
                    walllong->draw(texProg);
                Model->popMatrix();

                Model->pushMatrix();
                    texture10->bind(texProg->getUniform("Texture0"));
                    Model->translate(vec3(-40.5f, 2, 7.5f));
                    Model->rotate(PI/2, vec3(0, 1, 0));
                    Model->scale(vec3(3.5f, 3.5f, 3.5f));
                    resize_and_center(walllong->min, walllong->max, Model);
                    setModel(texProg, Model);
                    walllong->draw(texProg);
                Model->popMatrix();

            Model->popMatrix();
           
            //tiled wall long one
            Model->pushMatrix();
                Model->translate(vec3(-24, 1, 10));
                Model->rotate(PI/2, vec3(0, 1, 0));
                Model->scale(vec3(16, 14, 5));
                texture11->bind(texProg->getUniform("Texture0"));
                resize_and_center(tiledwall->min, tiledwall->max, Model);
                setModel(texProg, Model);
                tiledwall->draw(texProg);
            Model->popMatrix();


            //back tiled wall
            Model->pushMatrix();
                Model->translate(vec3(-10, 1, 26));
                Model->scale(vec3(14, 14, 5));
                texture11->bind(texProg->getUniform("Texture0"));
                resize_and_center(tiledwall->min, tiledwall->max, Model);
                setModel(texProg, Model);
                tiledwall->draw(texProg);
            Model->popMatrix();

            //lower crate
            Model->pushMatrix();
                Model->translate(vec3(-17, 0.1f, 20));
                Model->rotate(PI/2, vec3(0, 1, 0));
                Model->scale(vec3(6, 6, 6));
                texture12->bind(texProg->getUniform("Texture0"));
                resize_and_center(crate->min, crate->max, Model);
                setModel(texProg, Model);
                crate->draw(texProg);
            Model->popMatrix();

            Model->pushMatrix();
                Model->translate(vec3(-9, 6.6f, 20));
                Model->rotate(PI/5, vec3(0, 0, 1));
                Model->rotate(-PI/2.3, vec3(0, 1, 0));
                Model->scale(vec3(6, 6, 6));
                texture12->bind(texProg->getUniform("Texture0"));
                resize_and_center(crate->min, crate->max, Model);
                setModel(texProg, Model);
                crate->draw(texProg);
            Model->popMatrix();



            //left corner
            Model->pushMatrix();
                Model->translate(vec3(40, 5, -14));
                Model->rotate(-PI/3, vec3(0, 1, 0));
                Model->scale(vec3(6, 6, 6));
                texture13->bind(texProg->getUniform("Texture0"));
                resize_and_center(telephone_pole->min, telephone_pole->max, Model);
                setModel(texProg, Model);
                telephone_pole->draw(texProg);
            Model->popMatrix();


            //right corner
            Model->pushMatrix();
                Model->translate(vec3(-40, 5, -14));
                Model->rotate(-PI/3, vec3(0, 1, 0));
                Model->scale(vec3(6, 6, 6));
                texture13->bind(texProg->getUniform("Texture0"));
                resize_and_center(telephone_pole->min, telephone_pole->max, Model);
                setModel(texProg, Model);
                telephone_pole->draw(texProg);
            Model->popMatrix();


            //on left of house
            Model->pushMatrix();
                Model->translate(vec3(24.5f, 5, -5));
                Model->rotate(-PI/3, vec3(0, 1, 0));
                Model->scale(vec3(6, 6, 6));
                texture13->bind(texProg->getUniform("Texture0"));
                resize_and_center(telephone_pole->min, telephone_pole->max, Model);
                setModel(texProg, Model);
                telephone_pole->draw(texProg);
            Model->popMatrix();

            //on right of storage unti
            Model->pushMatrix();
                Model->translate(vec3(-24.5f, 5, -5));
                Model->rotate(-PI/3, vec3(0, 1, 0));
                Model->scale(vec3(6, 6, 6));
                texture13->bind(texProg->getUniform("Texture0"));
                resize_and_center(telephone_pole->min, telephone_pole->max, Model);
                setModel(texProg, Model);
                telephone_pole->draw(texProg);
            Model->popMatrix();

            //scaffolding
            Model->pushMatrix();
                Model->translate(vec3(-21, -2, 5));
                Model->rotate(PI/2, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                texture12->bind(texProg->getUniform("Texture0"));
                resize_and_center(scaffolding->min, scaffolding->max, Model);
                setModel(texProg, Model);
                scaffolding->draw(texProg);
            Model->popMatrix();


            Model->pushMatrix();
                resize_and_center(wire->min, wire->max, Model);
                setModel(texProg, Model);
                wire->draw(texProg);
            Model->popMatrix();


            //stair building 
            Model->pushMatrix();
                texture14->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(13, -2, 30));
                Model->rotate(-PI/2, (vec3(0, 1, 0)));
                Model->scale(vec3(7, 7, 7));
                resize_and_center(stair_build->min, stair_build->max, Model);
                setModel(texProg, Model);
                stair_build->draw(texProg);
            Model->popMatrix();



        Model->popMatrix();


        
        texProg->unbind();




		//switch shaders to the texture mapping shader and draw the ground
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(texProg->getUniform("lightDirection"), 1, -1, 1); 
		glUniform1i(texProg->getUniform("flip"), 1); 
				
		drawGround(texProg);

		texProg->unbind();
		
		//animation update example
		sTheta = sin(glfwGetTime());
		cTheta = cos(glfwGetTime());
		eTheta = std::max(0.0f, (float)sin(glfwGetTime()));
		hTheta = std::max(0.0f, (float)cos(glfwGetTime()));

		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();

	}

};



int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
        float dt = application->calculateDeltaTime();
        dt = std::fmin(dt, 0.03);

        application->drone.updatePosition(dt);
		// Render scene.
		application->render();
    
        windowManager->pollGamepadInput();
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
