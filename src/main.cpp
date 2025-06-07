/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CSC 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */



//TODO
//finish scene
//fix OBB
//add collectable propellers
//fix text class

//I could make a shape array to load each shape and then have enums
//for the index with the name being the index for that object
//TODO 
//make it so the obstacles are cell shaded solid colors
//Text generation using a namespace
//implement OBB
//
//
//I could pass  the bboxProg to draw_and_collid eto also draw the bbox
#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "Drone.h"
#include "Physics.h"
#include "AABB.h"
#include "Spline.h"
#include "OBB.h"
#include "Text.h"

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

	std::shared_ptr<Program> textProg;

	std::shared_ptr<Program> bboxProg;

	//Our shader program for textures
	std::shared_ptr<Program> texProg;

	//our geometry
	shared_ptr<Shape> sphere;




	shared_ptr<Shape> cementwall;

	shared_ptr<Shape> farground;



	shared_ptr<Shape> metalfence;

	shared_ptr<Shape> walllong;

	shared_ptr<Shape> telephone_pole;

	shared_ptr<Shape> bunny;



	shared_ptr<Shape> wire;

    shared_ptr<Shape> skyscraper;

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

    shared_ptr<Texture> texture15;

    shared_ptr<Texture> texture16;

    shared_ptr<Texture> texture17;



    vector <shared_ptr<Shape>> shapes;

    map<char, Character> characters;

    //example data that might be useful when trying to compute bounds on multi-shape
    vec3 gMin;
    vec3 gMax;
    vec3 gPos;
    vec3 gCenter = vec3(0, 0, 0);
    float radius = 100;



    float phi = 0.0f;
    float theta = PI/2;
    float roll = 0;

    bool gamepad_connected;
    //gamepad 
    float yawDelta = 0;
    float pitchDelta = 0;
    float rollDelta = 0;

    //global data (larger program should be encapsulated)
    float gRot = 0;
    float gCamH = 0;
    float sensitivity = .1 ;
    //animation data
    float lightTrans = 0;
    float gTrans = -3;
    float sTheta = 0;
    float cTheta = 0;
    float eTheta = 0;
    float hTheta = 0;


    vector<shared_ptr<AABB>> draw_boxes;
    bool goCamera = true;
    

    Spline splinepath[3];
    int currentSpline = 0;
    int numSplines = 3;


    struct Material{
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };

    struct multiModel{
        vector<shared_ptr<Shape>> shapes;
        vector<shared_ptr<AABB>> AABB_boxes;
        vector<shared_ptr<OBB>> OBB_boxes;
        int isStatic;
        vec3 gMin;
        vec3 gMax;

        //implement cache

        //we need this to be able to loop through the boxes and shapes drawing
        //each model along with transforming the boxes and creating copies for
        //each box so that the previous ones arent overwritten
        void draw_and_collide(shared_ptr<Program> prog, mat4 Model, Drone &drone){
            if(shapes.size() == AABB_boxes.size() || shapes.size() == OBB_boxes.size()){
                for(int i = 0; i < shapes.size(); i++){
                   shapes[i]->draw(prog); 
                  
                   if(OBB_boxes.size() > 0){
                       OBB transformedBox = OBB_boxes[i]->transformed(mat4(1));
                       Physics::handleCollision(transformedBox, drone, Model);
                   }else{
                       AABB transformedBox = AABB_boxes[i]->transformed(Model);
                       Physics::handleCollision(transformedBox, drone);
                   }
                   //create a copy of box and push to enable multiple of the same
                   //AABB
                   //AABB transformedBox = boxes[i]->transformed(Model);
                   //transformedBox.init();
                   //causing lots of lag
                   //transformedBox.init();
                }
            }
        }
    };


    //copy the instances
    struct singleModel{
        shared_ptr<Shape> shape;
        shared_ptr<AABB> AABB_box;
        shared_ptr<OBB> OBB_box;
        int isstatic;
        vec3 gMin;
        vec3 gMax;

        //we need this to be able to loop through the boxes and shapes drawing
        //each model along with transforming the boxes and creating copies for
        //each box so that the previous ones arent overwritten
        void draw_and_collide(shared_ptr<Program> prog, mat4 Model, Drone &drone){
            shape->draw(prog);
            //create a copy of box and push to enable multiple of the same
            //AABB

            if(AABB_box){
                AABB transformedBox = AABB_box->transformed(Model);
                Physics::handleCollision(transformedBox, drone);
            }else{
                OBB transformedbox = OBB_box->transformed(mat4(1));
                Physics::handleCollision(transformedbox, drone, Model);
            }
        }
    };

    multiModel stair_building; 
    multiModel guardrail; 
    multiModel house;
    multiModel scaffolding;
    multiModel storageunit;
    multiModel cylinder1;
    multiModel crate;
    multiModel crane;
    singleModel ground;
    singleModel pallet;
    singleModel tiledwall;


    Drone drone;

    AABB worldBox = AABB(vec3(-200,-20,-200), vec3(200, 200, 200));

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

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{

        vec3 up = drone.orientation * vec3(0, 1, 0);
        vec3 front = drone.orientation * vec3(0, 0, -1);
        vec3 right = cross(up, front);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		//update global camera rotate
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {

            drone.position += right * vec3(2);
		}

		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            drone.position -= right * vec3(2);
		}

		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		   

            drone.position += front * vec3(2);
		}

		//update camera height
		if (key == GLFW_KEY_S && action == GLFW_PRESS){
            vec3 front = drone.orientation * vec3(0, 0, -1);

            drone.position -= front * vec3(2);
		}
		if (key == GLFW_KEY_G && action == GLFW_PRESS){
            goCamera = !goCamera;
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
       //drone.updateMouseOrientation(phi, theta, .005);
    }

    //stold this from betaflight :p
    float get_rate(float stick_input, float rcRate, float superRate, float baseDegPerSec = 200.0f) {
        float abs_input = fabs(stick_input);
        float base = stick_input * rcRate;
        float super = 1.0f / (1.0f - abs_input * superRate);
        float rate_deg = base * super * baseDegPerSec;
        float maxRate = rcRate * (1 / (1 - superRate)) * baseDegPerSec; 
        //printf("%f\n", maxRate);
        return glm::radians(rate_deg); 
    }
    //gather the controller inputs on callback
    void gamepadInputCallback(float leftX, float leftY, float rightX, float rightY, bool gamepad){
        gamepad_connected = gamepad;
        if(gamepad){
            //turn controller axie location into drone movement data
            drone.yawInput   = -leftX;
            drone.pitchInput =  rightY;
            drone.rollInput  =  rightX; //clamp throttle [0, 1]
            drone.throttle = (leftY+1)/2;
        }
    }


    void updateCamera(shared_ptr<MatrixStack> &view, Drone &drone){
        vec3 direction = drone.orientation * vec3(0, 0, -1);
        vec3 eye = drone.position;
        vec3 up = drone.orientation * vec3(0, 1, 0);
        direction = glm::normalize(direction);

        //eye + direction for look at because we need look at relative 
        //to where camera is 
        //view->lookAt(eye, eye+direction, vec3(0, 1, 0));
    }


    float calculateDeltaTime(){
        using clock = chrono::high_resolution_clock;
        static auto lastTime = clock::now();

        auto currentTime = clock::now();
        chrono::duration<float> delta = currentTime - lastTime;
        lastTime = currentTime;

        return delta.count();
    }


    void updateUsingCameraPath(float frametime)  {

   	  if (goCamera) {
       if(!splinepath[currentSpline].isDone()){
       		splinepath[currentSpline].update(frametime);
            gPos = splinepath[currentSpline].getPosition();

            if(currentSpline == 0){
                gCenter = vec3(0, 0, 0);
            }else if(currentSpline == 1){
                gCenter = vec3(-65, 20, 10);
            }else{
                gCenter = vec3(110, 20, 10);
            }
        } else {
            currentSpline =(currentSpline + 1) % numSplines;
            splinepath[currentSpline].reset();
        }
      }
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
       
        if(goCamera){
            view->lookAt(gPos, gCenter, vec3(0, 1, 0));
        }else{
            view->lookAt(eye, eye+forward, up);
        }
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

        splinepath[0] = Spline(glm::vec3(-radius, 10, -radius), glm::vec3(-radius,15,-radius), glm::vec3(radius, 15, -radius), glm::vec3(radius,10,-radius), 5);
        splinepath[1] = Spline(glm::vec3(-45, 20, 10), glm::vec3(0), glm::vec3(0), glm::vec3(-45, 20, -10), 10);
        splinepath[2] = Spline(glm::vec3(150, 10, 10), glm::vec3(150, 10, 10), glm::vec3(150, 10, -20), glm::vec3(150, 10, -20), 10);
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

        textProg = make_shared<Program>();
        textProg->setVerbose(true);
        textProg->setShaderNames(resourceDirectory + "/text_vert.glsl", resourceDirectory + "/text_frag.glsl");
        textProg->init();
        textProg->addUniform("P");
        textProg->addUniform("text");
        textProg->addUniform("textColor");
        textProg->addAttribute("vertex");





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

        bboxProg = make_shared<Program>();
        bboxProg->setVerbose(true);
        bboxProg->setShaderNames(resourceDirectory + "/silhoutte_vert.glsl", resourceDirectory + "/silhoutte_frag.glsl");
        bboxProg->init();
        bboxProg->addUniform("P");
        bboxProg->addUniform("V");
        bboxProg->addUniform("M");
        bboxProg->addAttribute("vertPos");


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

		texture15 = make_shared<Texture>();
  		texture15->setFilename(resourceDirectory + "/tileblock.png");
  		texture15->init();
  		texture15->setUnit(0);
  		texture15->setWrapModes(GL_REPEAT, GL_REPEAT);
        texture15->setFiltering(GL_NEAREST, GL_NEAREST);

		texture16 = make_shared<Texture>();
  		texture16->setFilename(resourceDirectory + "/skyscraper.png");
  		texture16->init();
  		texture16->setUnit(0);
  		texture16->setWrapModes(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
        texture16->setFiltering(GL_NEAREST, GL_NEAREST);

		texture17 = make_shared<Texture>();
  		texture17->setFilename(resourceDirectory + "/scaffoldingblue.png");
  		texture17->init();
  		texture17->setUnit(0);
  		texture17->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture17->setFiltering(GL_NEAREST, GL_NEAREST);

	}

	void initGeom(const std::string& resourceDirectory)
	{

        Text::load_characters(characters);

        worldBox.init();
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







		vector<tinyobj::shape_t> TOshapesA;
 		vector<tinyobj::material_t> objMaterialsA;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesA, objMaterialsA, errStr, (resourceDirectory + "/ground.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			farground= make_shared<Shape>();
			farground->createShape(TOshapesA[0]);
			farground->measure();
			farground->init();
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




		vector<tinyobj::shape_t> TOshapesR;
 		vector<tinyobj::material_t> objMaterialsR;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesR, objMaterialsR, errStr, (resourceDirectory + "/skyscraper.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			
			skyscraper= make_shared<Shape>();
		    skyscraper->createShape(TOshapesR[0]);
			skyscraper->measure();
			skyscraper->init();
		}


        house = loadMultiShape("/multi_shape_house.obj", resourceDirectory);
        stair_building= loadMultiShape("/multi_shape_stair.obj", resourceDirectory);
        guardrail= loadMultiShape("/multi_shape_guardrail.obj", resourceDirectory);
        scaffolding = loadMultiShape("/scaffolding_multi_shape.obj", resourceDirectory);
        storageunit = loadMultiShape("/multi_shape_storageunit.obj", resourceDirectory);
        crate = loadMultiShape("/storagecrate.obj", resourceDirectory, true);
        cylinder1= loadMultiShape("/multi_shape_cylinder.obj", resourceDirectory,true);
        crane = loadMultiShape("/multi_crane.obj", resourceDirectory);

        ground = loadSingleShape("/ground.obj", resourceDirectory);
        tiledwall = loadSingleShape("/tiledwall.obj", resourceDirectory);
        pallet = loadSingleShape("/pallet.obj", resourceDirectory, true);

		//code to load in the ground plane (CPU defined data passed to GPU)
		initGround();
	}

    multiModel loadMultiShape(const string &filepath, const string &resourceDirectory, bool OBB_flag=false){
        multiModel result;    

		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;

        vec3 minBounds = vec3(numeric_limits<float>::max());
        vec3 maxBounds = vec3(-numeric_limits<float>::max());
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + filepath).c_str());
       

		if (!rc) {
			cerr << errStr << endl;
		} else {
            for(int i = 0; i <TOshapes.size(); i++ ){
                auto shape = make_shared<Shape>();
                shape->createShape(TOshapes[i]);
                shape->measure();
                shape->init();
                result.shapes.push_back(shape);
                if(OBB_flag){
                    vec3 center = (shape->min + shape->max) * 0.5f; 
                    vec3 halfWidths = (shape->max - shape->min) * 0.5f;
                    mat3 orientation = mat3(1.0f);
                    auto box = make_shared<OBB>(center, halfWidths, orientation);
                    box->initAxes();
                    result.OBB_boxes.push_back(box);

                }else{
                    auto box = make_shared<AABB>(shape->min, shape->max);
                    box->init();
                    result.AABB_boxes.push_back(box);
                }

                //right now automatically adds to collision detection
                //might want to not automatically and have this on
                //draw

                minBounds.x = std::min(minBounds.x, shape->min.x);
                minBounds.y = std::min(minBounds.y, shape->min.y);
                minBounds.z = std::min(minBounds.z, shape->min.z);

                maxBounds.x = std::max(maxBounds.x, shape->max.x);
                maxBounds.y = std::max(maxBounds.y, shape->max.y);
                maxBounds.z = std::max(maxBounds.z, shape->max.z);
            } 
        }

        result.gMin = minBounds;
        result.gMax = maxBounds;
        return result;
    }

    singleModel loadSingleShape(const string &filepath, const string &resourceDirectory, bool OBB_flag=false){
        singleModel result;

 		string errStr;
		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + filepath).c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			auto shape = make_shared<Shape>();
			shape->createShape(TOshapes[0]);
			shape->measure();
			shape->init();
            if(OBB_flag){
                vec3 center = (shape->min + shape->max) * 0.5f; 
                vec3 halfWidths = (shape->max - shape->min) * 0.5f;
                mat3 orientation = mat3(1.0f);

                auto box = make_shared<OBB>(center, halfWidths, orientation);
                box->initAxes();
                result.OBB_box = box;
                
            }else{
                auto box = make_shared<AABB>(shape->min, shape->max);
                box->init();
                result.AABB_box = box;
            }
            result.shape = shape;
        }
        return result;
    }

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = 400;
		float g_groundY = -20;

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


	void render(float dt) {
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

        float yawVel   = get_rate(drone.yawInput,   drone.rcRate, drone.superRate);
        float pitchVel = get_rate(drone.pitchInput, drone.rcRate, drone.superRate);
        float rollVel  = get_rate(drone.rollInput,  drone.rcRate, drone.superRate);

        if(goCamera){
            updateUsingCameraPath(dt);

        }else{
            drone.updatePosition(dt);
            drone.updateOrientation(rollVel, pitchVel, yawVel, dt);
            drone.updateTrickState(dt);
        }

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.3f, aspect, 0.01f, 800.0f);

		// View is global translation along negative z for now
		View->pushMatrix();
		View->loadIdentity();
            
        updateCamera(View, drone.position, drone.orientation, drone.camera_title_angle);
       
        glm::mat4 P_ortho= glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

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
            Model->rotate(PI/2, vec3(0, 1, 0));
            Model->scale(vec3(600, 600, 600));
            setModel(texProg, Model);
            sphere->draw(texProg);
        Model->popMatrix();
        texProg->unbind();

        bboxProg->bind();
		glUniformMatrix4fv(bboxProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(bboxProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
        bboxProg->unbind();


		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), 2.0+lightTrans, 2.0+lightTrans, 2.9+lightTrans);
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
            Model->scale(vec3(4, 4, 4));


            //cylinder  in front
            Model->pushMatrix();
                texture3->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-3.7, -.1f, 2));
                Model->rotate(-PI/6, vec3(0, 1, 0));//turn
                Model->rotate(PI/2.7, vec3(1, 0, 0));//rotate on side
                Model->rotate(PI/2, vec3(0, 0, 1));//flip over
                Model->scale(vec3(1.2f,1.2f,1.2f));

                //Model->scale(vec3(1, ));
                resize_and_center(cylinder1.gMin, cylinder1.gMax, Model);
                setModel(texProg, Model);
                cylinder1.draw_and_collide(texProg, Model->topMatrix(), drone);
                
            Model->popMatrix();


            //cylinder in back
            Model->pushMatrix();
                texture3->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-7, -.2f, 1.5));
                Model->rotate(PI/2.7, vec3(1, 0, 0));//rotate on side
                Model->rotate(PI/2, vec3(0, 0, 1));
                Model->scale(vec3(.9f,.9f,.9f));
                resize_and_center(cylinder1.gMin, cylinder1.gMax, Model);
                setModel(texProg, Model);
                cylinder1.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();


            //cylinder along wall
            Model->pushMatrix();
                texture3->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(2, -.2f, 3));
                Model->rotate(PI/2, vec3(0, 1, 0));//turn
                Model->rotate(PI/2.7, vec3(1, 0, 0));//rotate on side
                Model->rotate(PI/2, vec3(0, 0, 1));
                Model->scale(vec3(1, 3,1));
                resize_and_center(cylinder1.gMin, cylinder1.gMax, Model);
                setModel(texProg, Model);
                cylinder1.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();

            //ground
            Model->pushMatrix();
                Model->translate(vec3(0, -5, 5));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(40, 10, 40));
                texture2->bind(texProg->getUniform("Texture0"));
                resize_and_center(ground.shape->min, ground.shape->max, Model);
                setModel(texProg, Model);
                ground.draw_and_collide(texProg, Model->topMatrix(), drone);

            Model->popMatrix();




            //house to left
            Model->pushMatrix();
                texture0->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(13, 10, -2));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(8, 8, 8));
                resize_and_center(house.gMin, house.gMax, Model);
                setModel(texProg, Model);
                house.draw_and_collide(texProg, Model->topMatrix(), drone);
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
                resize_and_center(storageunit.gMin, storageunit.gMax, Model);
                setModel(texProg, Model);
                storageunit.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();

            //guard rail middle left1
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(0, -.35f, -18));
                Model->rotate(-PI/2.1, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail.gMin, guardrail.gMax, Model);
                setModel(texProg, Model);
                guardrail.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();

            //guard rail middle right
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(12, -.35f, -18));
                Model->rotate(-PI/1.9, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail.gMin, guardrail.gMax, Model);
                setModel(texProg, Model);
                guardrail.draw_and_collide(texProg, Model->topMatrix(), drone);

            Model->popMatrix();

            //guard rail most right
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(28, -.35f, -18));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail.gMin, guardrail.gMax, Model);
                setModel(texProg, Model);
                guardrail.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();


            //guard rail most left
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-30, -.35f, -18));
                Model->rotate(-PI/2.1, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail.gMin, guardrail.gMax, Model);
                setModel(texProg, Model);
                guardrail.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();


            //guard rail left
            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-18, -.35f, -18));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->scale(vec3(5, 5, 5));
                resize_and_center(guardrail.gMin, guardrail.gMax, Model);
                setModel(texProg, Model);
                guardrail.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();

            //pallet laying on top
            Model->pushMatrix();
                texture8->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(2.5f, .5f, -4));
                Model->rotate(-PI/2, vec3(0, 1, 0));
                Model->rotate(PI/3, vec3(1, 0, 0));
                Model->scale(vec3(2, 2.5f, 2));
                resize_and_center(pallet.shape->min, pallet.shape->max, Model);
                setModel(texProg, Model);
                pallet.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();

            //pallet on floor
            Model->pushMatrix();
                texture8->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-20, -.8f, -3));
                Model->rotate(PI/4, vec3(0, 1, 0));
                Model->scale(vec3(2, 3, 2));
                resize_and_center(pallet.shape->min, pallet.shape->max, Model);
                setModel(texProg, Model);
                pallet.draw_and_collide(texProg, Model->topMatrix(), drone);

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
                resize_and_center(tiledwall.shape->min, tiledwall.shape->max, Model);
                setModel(texProg, Model);
                tiledwall.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();


            //back tiled wall
            Model->pushMatrix();
                Model->translate(vec3(-10, 1, 26));
                Model->scale(vec3(14, 14, 5));
                texture11->bind(texProg->getUniform("Texture0"));
                resize_and_center(tiledwall.shape->min, tiledwall.shape->max, Model);
                setModel(texProg, Model);
                tiledwall.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();



            //lower crate
            Model->pushMatrix();
                Model->translate(vec3(-17, 0.3f, 20));
                Model->rotate(PI/2, vec3(0, 1, 0));
                Model->scale(vec3(6, 6, 6));
                texture12->bind(texProg->getUniform("Texture0"));
                resize_and_center(crate.gMin, crate.gMax, Model);
                setModel(texProg, Model);
                crate.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();


            // crate hanging
            Model->pushMatrix();
                Model->translate(vec3(-20, 14.5f, 13));
                Model->rotate(PI/2, vec3(0, 1, 0));
                Model->rotate(sTheta*.1f, vec3(1, 0, 1));

                Model->scale(vec3(6, 6, 6));
                texture17->bind(texProg->getUniform("Texture0"));
                resize_and_center(crate.gMin, crate.gMax, Model);
                setModel(texProg, Model);
                crate.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();

            Model->pushMatrix();
                Model->translate(vec3(-9, 6.8f, 20));
                Model->rotate(PI/5, vec3(0, 0, 1));
                Model->rotate(-PI/2.5, vec3(0, 1, 0));
                Model->scale(vec3(6, 6, 6));
                texture12->bind(texProg->getUniform("Texture0"));
                resize_and_center(crate.gMin, crate.gMax, Model);
                setModel(texProg, Model);
                crate.draw_and_collide(texProg, Model->topMatrix(), drone);
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
                resize_and_center(scaffolding.gMin, scaffolding.gMax, Model);
                setModel(texProg, Model);
                scaffolding.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();


            Model->pushMatrix();
                resize_and_center(wire->min, wire->max, Model);
                setModel(texProg, Model);
                wire->draw(texProg);
            Model->popMatrix();


            /*
            //stair building 
            Model->pushMatrix();
                texture14->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(13, -4, 30));
                Model->rotate(-PI/2, (vec3(0, 1, 0)));
                Model->scale(vec3(7, 7, 7));
                resize_and_center(stair_build->min, stair_build->max, Model);
                setModel(texProg, Model);
                stair_build->draw(texProg);
            Model->popMatrix();
            */


            Model->pushMatrix();
                texture14->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(13, -4, 30));
                Model->rotate(-PI/2, (vec3(0, 1, 0)));
                Model->scale(vec3(7, 7, 7));
                resize_and_center(stair_building.gMin, stair_building.gMax, Model);
                setModel(texProg, Model);
                stair_building.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();


            //far ground left most right
            Model->pushMatrix();
                texture15->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-95, 1, -70));
                Model->rotate(-PI/4, vec3(0, 1, 0));
                Model->scale(vec3(50, 20, 50));
                resize_and_center(farground->min, farground->max, Model);
                setModel(texProg, Model);
                farground->draw(texProg);
            Model->popMatrix();


            Model->pushMatrix();
                texture15->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-105, 3, -8));
                Model->scale(vec3(30, 40, 30));
                resize_and_center(farground->min, farground->max, Model);
                setModel(texProg, Model);
                farground->draw(texProg);
                Model->pushMatrix();
                    Model->translate(vec3(7, 0, 14));
                    Model->rotate(PI/4, vec3(0, 1, 0));
                    setModel(texProg, Model);
                    farground->draw(texProg);
                Model->popMatrix();
            Model->popMatrix();


            //flipped farground
            Model->pushMatrix();
                texture15->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(95, 1, -70));
                Model->rotate(PI/4, vec3(0, 1, 0));
                Model->scale(vec3(50, 20, 50));
                resize_and_center(farground->min, farground->max, Model);
                setModel(texProg, Model);
                farground->draw(texProg);
            Model->popMatrix();


            Model->pushMatrix();
                texture15->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(105, 3, -8));
                Model->scale(vec3(30, 40, 30));
                resize_and_center(farground->min, farground->max, Model);
                setModel(texProg, Model);
                farground->draw(texProg);
                Model->pushMatrix();
                    Model->translate(vec3(-7, 0, 14));
                    Model->rotate(-PI/4, vec3(0, 1, 0));
                    setModel(texProg, Model);
                    farground->draw(texProg);
                Model->popMatrix();
            Model->popMatrix();

            Model->pushMatrix();
                texture16->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(-70, 30, -60));
                Model->scale(vec3(20, 20, 20));
                resize_and_center(skyscraper->min, skyscraper->max, Model);
                setModel(texProg, Model);
                skyscraper->draw(texProg);
            Model->popMatrix();


            Model->pushMatrix();
                texture7->bind(texProg->getUniform("Texture0"));
                Model->translate(vec3(15, 7, 13));
                Model->rotate(PI/2, vec3(0, 1, 0));
                Model->scale(vec3(10, 10, 10));
                Model->pushMatrix();
                resize_and_center(crane.gMin, crane.gMax, Model);
                setModel(texProg, Model);
                crane.draw_and_collide(texProg, Model->topMatrix(), drone);
            Model->popMatrix();


        Model->popMatrix();


        
        texProg->unbind();





        Physics::clampToWorld(worldBox, drone);
        


		//switch shaders to the texture mapping shader and draw the ground
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(texProg->getUniform("lightDirection"), 1, -1, 1); 
		glUniform1i(texProg->getUniform("flip"), 1); 
				
		drawGround(texProg);

		texProg->unbind();


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        textProg->bind();
        glUniform1i(textProg->getUniform("text"), 0);
        glUniformMatrix4fv(textProg->getUniform("P"), 1, GL_FALSE, value_ptr(P_ortho));
        if(goCamera){
            Text::RenderText(textProg, "WHOOPS AND HOOPS", 100, 400, .1*sTheta+1, glm::vec3(1, 1, 1), characters);
            if(!gamepad_connected){
                Text::RenderText(textProg, "NO GAMEPAD DETECTED!", 200, 200, .1*sTheta+1, glm::vec3(1, 0, 0), characters);
            }
        }else if (!goCamera && gamepad_connected){
            int speed = static_cast<int>(length(drone.velocity));
            Text::RenderText(textProg, string( "SPEED: " + to_string(speed)), 25.0f, 25.0f, .5f, glm::vec3(0.5, 0.8f, 0.2f), characters);
            Text::RenderText(textProg, "ACRO", 25.0f, 75.0f, .5f, glm::vec3(0.5, 0.8f, 0.2f), characters);
            Text::RenderText(textProg, drone.trick , 25.0f, 125.0f, 1, glm::vec3(0.5, 0.8f, 0.2f), characters);
        }else if(!goCamera && !gamepad_connected){
            if(!gamepad_connected){
                Text::RenderText(textProg, "NO GAMEPAD DETECTED!", 100, 400, .1*sTheta+1, glm::vec3(1, 0, 0), characters);
            }
        }
        textProg->unbind();
        glDisable(GL_BLEND);

        
		
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


		// Render scene.
		application->render(dt);
    
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
