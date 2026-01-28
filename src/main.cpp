// TODO fix in scene wher colliders are hard coded move by parent offset
// poitners to A timer must be introduced that spawns them after one is
// colelectd
#include <chrono>
#include <glad/glad.h>
#include <iostream>

#include "AABB.h"
#include "Drone.h"
#include "EntityProcess.h"
#include "GLSL.h"
#include "Hud.h"
#include "Lipo.h"
#include "MatrixStack.h"
#include "OBB.h"
#include "Physics.h"
#include "PhysicsWorld.h"
#include "Program.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Shape.h"
#include "Spline.h"
#include "Text.h"
#include "Texture.h"
#include "WindowManager.h"
#include "ocean.h"
#include "skybox.h"

#define PI 3.1415926535

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks {

public:
  // Hud elements
  Hud hud;
  HUDSprite style_meter;

  // we neeed to load in the faces for skybox
  std::vector<std::string> faces = {
      "/skybox/px.png", "/skybox/nx.png", "/skybox/py.png",
      "/skybox/ny.png", "/skybox/pz.png", "/skybox/nz.png",
  };
  Skybox skybox;
  Ocean ocean;

  // scene stuff
  Scene scene;
  ResourceManager resourceManager;
  PhysicsWorld physicsWorld;

  WindowManager *windowManager = nullptr;
  // render class
  // Our shader program
  std::shared_ptr<Program> textProg;
  std::shared_ptr<Program> texProg;
  std::shared_ptr<Program> solidProg;
  std::shared_ptr<Program> skyProg;

  // our geometry
  shared_ptr<Shape> sphere;
  shared_ptr<Shape> farground;
  shared_ptr<Shape> cube;
  shared_ptr<Shape> skyscraper;

  // global data for ground plane - direct load constant defined CPU data to GPU
  // (not obj)
  GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
  int g_GiboLen;
  // ground VAO
  GLuint GroundVertexArrayID;

  // the image to use as a texture (ground)
  shared_ptr<Texture> texture1;
  shared_ptr<Texture> texture5;
  shared_ptr<Texture> stylebar_sheet;
  shared_ptr<Texture> lipo_texture;
  map<char, Character> characters;
  float dt;

  // lipo
  std::shared_ptr<Lipo> lipo;
  std::shared_ptr<Lipo> lipo2;
  // vector<shared_ptr<Entity>> entities;
  EntityProcess entityProcess;

  // example data that might be useful when trying to compute bounds on
  // multi-shape
  vec3 gMin;
  vec3 gMax;
  vec3 gPos;
  vec3 gCenter = vec3(0, 0, 0);
  float radius = 100;

  float phi = 0.0f;
  float theta = PI / 2;
  float roll = 0;

  bool gamepad_connected;
  // gamepad
  float yawDelta = 0;
  float pitchDelta = 0;
  float rollDelta = 0;

  // global data (larger program should be encapsulated)
  float gRot = 0;
  float gCamH = 0;
  float sensitivity = .1;
  // animation data
  float gTrans = -3;
  float sTheta = 0;
  float cTheta = 0;
  float eTheta = 0;
  float hTheta = 0;
  bool debugCam = false;
  bool hud_flag = true;

  bool goCamera = true;
  bool gameOverFlag = false;

  Spline splinepath[3];
  int currentSpline = 0;
  int numSplines = 3;

  Drone drone;

  AABB worldBox = AABB(vec3(-170, -20, -170), vec3(170, 200, 250));

  void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                   int mods) {

    vec3 up = drone.orientation * vec3(0, 1, 0);
    vec3 front = drone.orientation * vec3(0, 0, -1);
    vec3 right = cross(up, front);
    float cameraSpeed = 2.5f * dt;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
      goCamera = !goCamera;
    }

    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
      drone.throttle = 1;
    }

    if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
      drone.throttle = 0;
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
      debugCam = !debugCam;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      if (gameOverFlag) {
        std::cout << "Restarting the game" << std::endl;
        restartGame();
      }
    }

    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
      hud_flag = !hud_flag;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }

  void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
    double posX, posY;
    drone.getPosition();
    glfwGetCursorPos(window, &posX, &posY);
    cout << "Pos X " << posX << " Pos Y " << posY << endl;
  }

  // gather the deltaX and deltaY on scroll and change the phi and theta
  // based off the sensitivity
  void scrollCallback(GLFWwindow *window, double deltaX, double deltaY) {
    if (debugCam) {
      phi -= deltaY * sensitivity;
      theta += deltaX * sensitivity;
      drone.updateMouseOrientation(phi, theta, .005);
    }
  }

  // stold this from betaflight :p
  float get_rate(float stick_input, float rcRate, float superRate,
                 float baseDegPerSec = 200.0f) {
    float abs_input = fabs(stick_input);
    float base = stick_input * rcRate;
    float super = 1.0f / (1.0f - abs_input * superRate);
    float rate_deg = base * super * baseDegPerSec;
    float maxRate = rcRate * (1 / (1 - superRate)) * baseDegPerSec;
    // printf("%f\n", maxRate);
    return glm::radians(rate_deg);
  }
  // gather the controller inputs on callback
  void gamepadInputCallback(float leftX, float leftY, float rightX,
                            float rightY, bool gamepad) {
    gamepad_connected = gamepad;
    if (gamepad) {
      // turn controller axie location into drone movement data
      drone.yawInput = -leftX;
      drone.pitchInput = rightY;
      drone.rollInput = rightX; // clamp throttle [0, 1]
      drone.throttle = (leftY + 1) / 2;
    }
  }

  void updateCamera(shared_ptr<MatrixStack> &view, Drone &drone) {
    vec3 direction = drone.orientation * vec3(0, 0, -1);
    vec3 eye = drone.position;
    vec3 up = drone.orientation * vec3(0, 1, 0);
    direction = glm::normalize(direction);
  }

  void calculateDeltaTime() {
    using clock = chrono::high_resolution_clock;
    static auto lastTime = clock::now();

    auto currentTime = clock::now();
    chrono::duration<float> delta = currentTime - lastTime;
    lastTime = currentTime;

    dt = delta.count();
    dt = std::fmin(dt, 0.03);
  }

  void updateUsingCameraPath(float frametime) {
    if (goCamera || gameOverFlag) {
      if (!splinepath[currentSpline].isDone()) {
        splinepath[currentSpline].update(frametime);
        gPos = splinepath[currentSpline].getPosition();

        if (currentSpline == 0) {
          gCenter = vec3(0, 0, 0);
        } else if (currentSpline == 1) {
          gCenter = vec3(-65, 20, 10);
        } else {
          gCenter = vec3(110, 20, 10);
        }
      } else {
        currentSpline = (currentSpline + 1) % numSplines;
        splinepath[currentSpline].reset();
      }
    }
  }

  // update camera location and orrientation based off drone
  void updateCamera(shared_ptr<MatrixStack> &view, vec3 drone_position,
                    quat drone_orientation, float drone_camera_angle) {
    // rotate around x axis to pitch
    quat cameraPitch = angleAxis(radians(drone_camera_angle), vec3(1, 0, 0));
    quat cameraOrientation = drone_orientation * cameraPitch;
    vec3 eye = drone_position;
    vec3 forward = cameraOrientation * vec3(0.0f, 0.0f, -1.0f);
    vec3 up = cameraOrientation * vec3(0.0f, 1.0f, 0.0f);
    // to where camera is
    if (goCamera || gameOverFlag) {
      view->lookAt(gPos, gCenter, vec3(0, 1, 0));
    } else {
      view->lookAt(eye, eye + forward, up);
    }
  }

  void resizeCallback(GLFWwindow *window, int width, int height) {
    sensitivity = 180.0f / height;
    glViewport(0, 0, width, height);
    hud.setScreenSize(width, height);
  }

  void init(const std::string &resourceDirectory) {

    GLSL::checkVersion();
    lipo = make_shared<Lipo>(resourceDirectory);
    entityProcess.add(lipo);
    // entities.push_back(lipo);

    // Set background color.
    glClearColor(.72f, .84f, 1.06f, 1.0f);
    // Enable z-buffer test.
    glEnable(GL_DEPTH_TEST);

    splinepath[0] = Spline(
        glm::vec3(-radius, 10, -radius), glm::vec3(-radius, 15, -radius),
        glm::vec3(radius, 15, -radius), glm::vec3(radius, 10, -radius), 5);
    splinepath[1] = Spline(glm::vec3(-45, 20, 10), glm::vec3(0), glm::vec3(0),
                           glm::vec3(-45, 20, -10), 10);
    splinepath[2] =
        Spline(glm::vec3(150, 10, 10), glm::vec3(150, 10, 10),
               glm::vec3(150, 10, -20), glm::vec3(150, 10, -20), 10);

    // solid program for drawing solid colored objects
    solidProg = make_shared<Program>();
    solidProg->setVerbose(true);
    solidProg->setShaderNames(resourceDirectory + "/shaders/solid_vert.glsl",
                              resourceDirectory + "/shaders/solid_frag.glsl");
    solidProg->init();
    solidProg->addUniform("P");
    solidProg->addUniform("M");
    solidProg->addUniform("color");
    solidProg->addAttribute("vertPos");
    solidProg->addAttribute("vertNor");

    // text program for drawing text to screen
    textProg = make_shared<Program>();
    textProg->setVerbose(true);
    textProg->setShaderNames(resourceDirectory + "/shaders/text_vert.glsl",
                             resourceDirectory + "/shaders/text_frag.glsl");
    textProg->init();
    textProg->addUniform("P");
    textProg->addUniform("M");
    textProg->addUniform("text");
    textProg->addUniform("textColor");
    textProg->addUniform("uvOffset");
    textProg->addUniform("uvSize");
    textProg->addAttribute("vertex");

    // Initialize the GLSL program that we will use for texture mapping
    texProg = make_shared<Program>();
    texProg->setVerbose(true);
    texProg->setShaderNames(resourceDirectory + "/shaders/tex_vert.glsl",
                            resourceDirectory + "/shaders/tex_frag0.glsl");
    texProg->init();
    texProg->addUniform("P");
    texProg->addUniform("V");
    texProg->addUniform("M");
    texProg->addUniform("lightDirection");
    texProg->addUniform("flip");
    texProg->addUniform("cameraPosition");
    texProg->addUniform("Texture0");
    texProg->addUniform("lightToggle");
    texProg->addAttribute("vertPos");
    texProg->addAttribute("vertNor");
    texProg->addAttribute("vertTex");

    skyProg = make_shared<Program>();
    skyProg->setVerbose(true);
    skyProg->setShaderNames(resourceDirectory + "/shaders/skyVS.glsl",
                            resourceDirectory + "/shaders/skyFS.glsl");
    skyProg->init();
    skyProg->addUniform("P");
    skyProg->addUniform("V");
    skyProg->addUniform("skybox");
    skyProg->addAttribute("vertPos");

    texture1 = make_shared<Texture>();
    texture1->setFilename(resourceDirectory + "/sky_28_2k.png");
    texture1->init();
    texture1->setUnit(1);
    texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    texture1->setFiltering(GL_NEAREST, GL_NEAREST);

    texture5 = make_shared<Texture>();
    texture5->setFilename(resourceDirectory + "/water.png");
    texture5->init();
    texture5->setUnit(0);
    texture5->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    texture5->setFiltering(GL_NEAREST, GL_NEAREST);

    stylebar_sheet = make_shared<Texture>();
    stylebar_sheet->setFilename(resourceDirectory + "/stylebar_sheet.png");
    stylebar_sheet->init();
    stylebar_sheet->setUnit(1);
    stylebar_sheet->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    stylebar_sheet->setFiltering(GL_NEAREST, GL_NEAREST);

    lipo_texture = make_shared<Texture>();
    lipo_texture->setFilename(resourceDirectory + "/1slipo.png");
    lipo_texture->init();
    lipo_texture->setUnit(0);
    lipo_texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    lipo_texture->setFiltering(GL_NEAREST, GL_NEAREST);

    // set up the scenes models, textures, and physics
    scene.load(resourceDirectory + "/scenes/scene.json", resourceManager);
    scene.setupPhysics(physicsWorld);

    style_meter = {
        stylebar_sheet,
        glm::vec2(45, 600),
        glm::vec2(170, 312),
        glm::vec3(0, 0, 0),
        0.0,
        0.0,
    };
    hud.init();
    hud.addSprite(style_meter);

    skybox.setFaces(faces);
    skybox.init();

    ocean.setResourceDir(resourceDirectory);
    ocean.init();
  }

  void initGeom(const std::string &resourceDirectory) {

    // we need to load the characters into the map datastructure for
    // text dispaly
    Text::load_characters(characters);

    // initialize the world bounding box
    worldBox.init();

    // load in the mesh and make the shape(s)
    vector<tinyobj::shape_t> TOshapesZ;
    vector<tinyobj::material_t> objMaterialsZ;
    string errStr;
    bool rc = tinyobj::LoadObj(TOshapesZ, objMaterialsZ, errStr,
                               (resourceDirectory + "/cube.obj").c_str());
    if (!rc) {
      cerr << errStr << endl;
    } else {
      cube = make_shared<Shape>();
      cube->createShape(TOshapesZ[0]);
      cube->measure();
      cube->init();
    }

    vector<tinyobj::shape_t> TOshapes;
    vector<tinyobj::material_t> objMaterials;
    // load in the mesh and make the shape(s)
    rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                          (resourceDirectory + "/sphereWTex.obj").c_str());
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
    // load in the mesh and make the shape(s)
    rc = tinyobj::LoadObj(TOshapesA, objMaterialsA, errStr,
                          (resourceDirectory + "/ground.obj").c_str());
    if (!rc) {
      cerr << errStr << endl;
    } else {

      farground = make_shared<Shape>();
      farground->createShape(TOshapesA[0]);
      farground->measure();
      farground->init();
    }

    vector<tinyobj::shape_t> TOshapesR;
    vector<tinyobj::material_t> objMaterialsR;
    // load in the mesh and make the shape(s)
    rc = tinyobj::LoadObj(TOshapesR, objMaterialsR, errStr,
                          (resourceDirectory + "/skyscraper.obj").c_str());
    if (!rc) {
      cerr << errStr << endl;
    } else {

      skyscraper = make_shared<Shape>();
      skyscraper->createShape(TOshapesR[0]);
      skyscraper->measure();
      skyscraper->init();
    }
  }

  /*sets the program passed model uniform to the MatrixStack passed*/
  void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M) {
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                       value_ptr(M->topMatrix()));
  }

  /*resizes the model into -1 to 1 range and centers at the origin*/
  void resize_and_center(vec3 gMin, vec3 gMax, shared_ptr<MatrixStack> Model) {
    float center_x = (gMax.x + gMin.x) / 2;
    float center_y = (gMax.y + gMin.y) / 2;
    float center_z = (gMax.z + gMin.z) / 2;

    float largest_extent = std::max(
        std::max((gMax.x - gMin.x), (gMax.y - gMin.y)), (gMax.z - gMin.z));
    float scale = 2.0 / largest_extent;
    Model->translate(vec3(-center_x, -center_y, -center_z));
    Model->scale(vec3(scale, scale, scale));
  }

  /*function to render the scene, dt is delta time*/
  void render() {
    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    glViewport(0, 0, width, height);

    // Clear framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float aspect = width / (float)height;

    // Create the matrix stacks
    auto Projection = make_shared<MatrixStack>();
    auto View = make_shared<MatrixStack>();
    auto Model = make_shared<MatrixStack>();

    // update Drone sates
    float yawVel = get_rate(drone.yawInput, drone.rcRate, drone.superRate);
    float pitchVel = get_rate(drone.pitchInput, drone.rcRate, drone.superRate);
    float rollVel = get_rate(drone.rollInput, drone.rcRate, drone.superRate);

    // Update Camera Based on Flags
    if (goCamera) {
      updateUsingCameraPath(dt);

    } else if (gameOverFlag) {
      updateUsingCameraPath(dt);
    } else {
      if (!debugCam) {
        drone.updatePosition(dt);
      }
      drone.updateOrientation(rollVel, pitchVel, yawVel, dt);
      drone.updateTrickState(dt);
    }

    // Apply perspective projection.
    Projection->pushMatrix();
    Projection->perspective(glm::radians(75.0f), aspect, 0.01f, 800.0f);
    // View is global translation along negative z for now
    View->pushMatrix();
    View->loadIdentity();
    updateCamera(View, drone.position, drone.orientation,
                 drone.camera_title_angle);

    glm::mat4 P_ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

    // draw skybox
    skyProg->bind();
    glDepthFunc(GL_LEQUAL);
    glUniformMatrix4fv(skyProg->getUniform("P"), 1, GL_FALSE,
                       value_ptr(Projection->topMatrix()));
    glm::mat4 skyboxView = glm::mat4(glm::mat3(View->topMatrix()));
    glUniformMatrix4fv(skyProg->getUniform("V"), 1, GL_FALSE,
                       value_ptr(skyboxView));
    glUniform1i(skyProg->getUniform("skybox"), 0);
    skybox.draw();
    glDepthFunc(GL_LESS);
    skyProg->unbind();

    // draw the ocean
    Model->pushMatrix();
    Model->loadIdentity();
    Model->translate(vec3(-800, -60, -900));
    Model->scale(vec3(100, 50, 100));
    ocean.render(Model->topMatrix(), View->topMatrix(), Projection->topMatrix(),
                 drone.position, glm::vec3(1.0f, -1.0f, 1.0f), glfwGetTime());
    Model->popMatrix();

    // Main scene
    texProg->bind();
    glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE,
                       value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE,
                       value_ptr(View->topMatrix()));
    glUniform3f(texProg->getUniform("lightDirection"), 1, -1, 1);
    glUniform3fv(texProg->getUniform("cameraPosition"), 1,
                 value_ptr(drone.position));
    glUniform1i(texProg->getUniform("flip"), 1);
    glUniform1i(texProg->getUniform("lightToggle"), 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Model->pushMatrix();
    Model->translate(vec3(0, 2, 0));
    Model->scale(vec3(4, 4, 4));
    // draw the scene
    scene.draw(texProg, Model->topMatrix());
    // draw the entities
    entityProcess.draw(texProg, Model);
    entityProcess.update(dt, drone); // TODO move this somewhere else
    // handle the drone collisions among all colliders
    physicsWorld.handleDroneCollisions(drone); // TODO move this somewhere else
    Model->popMatrix();
    texProg->unbind();
    /*we need this to restrict drone to worldBox*/
    Physics::clampToWorld(worldBox, drone);

    /*all of the text*/
    textProg->bind();
    glUniform1i(textProg->getUniform("text"), 0);
    glUniformMatrix4fv(textProg->getUniform("P"), 1, GL_FALSE,
                       value_ptr(P_ortho));

    if (debugCam) {
      Text::RenderText(textProg, "debug cam", 650, 550, .5, glm::vec3(1, 1, 1),
                       characters);
    }
    if (gameOverFlag) {
      Text::RenderText(textProg, string("Final Stats:"), 100, 550, .8f,
                       glm::vec3(1, 1, 1), characters);
      Text::RenderText(
          textProg,
          string("Score................." + to_string(drone.finalScore)), 150,
          500, .8f, glm::vec3(1, 1, 0), characters);
      Text::RenderText(textProg,
                       string("Time Alive............" + to_string(0)), 150,
                       450, .8f, glm::vec3(1, 1, 0), characters);
      Text::RenderText(
          textProg,
          string("Total Combos.........." + to_string(drone.totalCombos)), 150,
          400, .8f, glm::vec3(1, 1, 0), characters);
      Text::RenderText(
          textProg,
          string("Highest Combo........." + to_string(drone.highestCombo)), 150,
          350, .8f, glm::vec3(1, 1, 0), characters);
      Text::RenderText(textProg,
                       string("Batteries Collected..." +
                              to_string(drone.batteriesCollected)),
                       150, 300, .8f, glm::vec3(1, 1, 0), characters);
      Text::RenderText(
          textProg,
          string("Obstacles Hit........." + to_string(drone.obstaclesHit)), 150,
          250, .8f, glm::vec3(1, 1, 0), characters);

      Text::RenderText(textProg, "PRESS R TO TRY AGAIN", 400, 175,
                       .1 * sTheta + .7, glm::vec3(0, 1, 0), characters, 500,
                       true);
    }
    if (goCamera) {
      // main menu
      Text::RenderText(textProg, "WHOOPS AND HOOPS", 300, 500, .1 * sTheta + 1,
                       glm::vec3(1, 1, 1), characters, 500, true);
      Text::RenderText(textProg, "Press G to start", 250, 100, .7,
                       glm::vec3(0, 1, 0), characters);
    } else if (!goCamera && !gameOverFlag && hud_flag) {
      // main hud
      int speed = static_cast<int>(length(drone.velocity));

      // render information in bottom left
      Text::RenderText(textProg, string("SPEED: " + to_string(speed)), 25.0f,
                       25.0f, .75f, glm::vec3(0.5, 0.8f, 0.2f), characters);
      Text::RenderText(textProg, "ACRO", 25.0f, 75.0f, .75f,
                       glm::vec3(0.5, 0.8f, 0.2f), characters);
      Text::RenderText(
          textProg,
          string("BAT: " + to_string(static_cast<int>(drone.battery))), 25.0f,
          125.0f, .75f, glm::vec3(0.5, 0.8f, 0.2f), characters);

      // render score
      if (drone.trickCount > 0) {
        Text::RenderText(textProg,
                         string(to_string(drone.score) + " x " +
                                to_string(drone.trickCount)),
                         340.0f, 70.0f, .8f, glm::vec3(1, 1, 1), characters);
      }

      Text::RenderText(textProg,
                       string("Score: " + to_string(drone.totalScore)), 255.0f,
                       550.0f, .5f, glm::vec3(0, 0, 0), characters, 500, true);

      // render drone trick description
      Text::RenderText(textProg, drone.trick, 400.0f, 50.0f, .5f,
                       glm::vec3(1, 1, 0), characters, 500, true);
    }
    if (!gamepad_connected) {
      // gamepad disconnnected
      if (!gamepad_connected) {
        Text::RenderText(textProg, "NO GAMEPAD DETECTED!", 225, 50, .7,
                         glm::vec3(1, 0, 0), characters);
      }
    }
    textProg->unbind();
    glDisable(GL_BLEND);

    // draw and update hud
    if (!goCamera && !gameOverFlag) {
      float fill = drone.score / drone.special_score_thresh - dt;
      hud.setTargetFill(fill);
      hud.update(dt);
      hud.draw();
      if (drone.special_mode) {
        style_meter.size = glm::vec2(sTheta, cTheta);
      }
    }
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw the drone
    if (!goCamera && !gameOverFlag) {
      solidProg->bind();
      glUniformMatrix4fv(solidProg->getUniform("P"), 1, GL_FALSE,
                         value_ptr(Projection->topMatrix()));
      glUniform3f(solidProg->getUniform("color"), 0.0, 0.0, 1.0);
      Model->pushMatrix();

      static float propellerAngle = 0.0f;
      float spinSpeed = 360.0f * drone.throttle;
      propellerAngle += spinSpeed * dt;
      glm::quat fix = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));
      glm::quat fixedOrientation = drone.orientation * fix;
      glm::mat4 rot = glm::mat4_cast(fixedOrientation);

      glUniform3fv(solidProg->getUniform("color"), 1,
                   glm::value_ptr(drone.droneColor));
      Model->translate(vec3(1.8, -1.2, -2));
      Model->multMatrix(rot);
      Model->scale(vec3(.2, .05, .2));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->pushMatrix();
      glUniform3f(solidProg->getUniform("color"), 0.4, 0.4, 0.4);
      Model->translate(vec3(.2, 1.5, 0));
      Model->scale(vec3(.5, 2, .5));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();

      Model->pushMatrix();
      Model->translate(vec3(-.2, .5, 0));
      Model->rotate(PI / 3, vec3(0, 0, 1));
      Model->scale(vec3(1, 1, .3));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();
      Model->pushMatrix();
      glUniform3f(solidProg->getUniform("color"), 0.1, 0.1, 0.1);
      Model->translate(vec3(.4, 1.5, 0));
      Model->scale(vec3(.3, 1.5, .3));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();
      Model->pushMatrix();
      glUniform3fv(solidProg->getUniform("color"), 1,
                   glm::value_ptr(drone.droneColor));
      Model->translate(vec3(.7, -.1, .7));
      Model->scale(vec3(.8, .8, .8));
      setModel(solidProg, Model);
      cube->draw(solidProg);

      Model->pushMatrix();
      glUniform3f(solidProg->getUniform("color"), 1.0, 1.0, 1.0);
      Model->translate(vec3(0, .5, 0));
      Model->rotate(-propellerAngle, vec3(0, 1, 0));
      Model->scale(vec3(.3, .5, .3));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      // props accros
      Model->pushMatrix();
      Model->translate(vec3(0, .5, 0));
      Model->scale(vec3(4, .2, .5));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();
      Model->pushMatrix();
      Model->translate(vec3(0, .5, 0));
      Model->scale(vec3(.5, .2, 4));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();

      Model->popMatrix();
      Model->popMatrix();
      Model->pushMatrix();
      glUniform3fv(solidProg->getUniform("color"), 1,
                   glm::value_ptr(drone.droneColor));
      Model->translate(vec3(-.7, -.1, .7));
      Model->scale(vec3(.8, .8, .8));
      setModel(solidProg, Model);
      cube->draw(solidProg);

      Model->pushMatrix();
      glUniform3f(solidProg->getUniform("color"), 1.0, 1.0, 1.0);
      Model->translate(vec3(0, .5, 0));
      Model->rotate(-propellerAngle, vec3(0, 1, 0));
      Model->scale(vec3(.3, .5, .3));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      // props accros
      Model->pushMatrix();
      Model->translate(vec3(0, .5, 0));
      Model->scale(vec3(4, .2, .5));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();
      Model->pushMatrix();
      Model->translate(vec3(0, .5, 0));
      Model->scale(vec3(.5, .2, 4));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();

      Model->popMatrix();
      Model->popMatrix();
      Model->pushMatrix();
      glUniform3fv(solidProg->getUniform("color"), 1,
                   glm::value_ptr(drone.droneColor));
      Model->translate(vec3(.7, -.1, -.7));
      Model->scale(vec3(.8, .8, .8));
      setModel(solidProg, Model);
      cube->draw(solidProg);

      Model->pushMatrix();
      glUniform3f(solidProg->getUniform("color"), 1.0, 1.0, 1.0);
      Model->translate(vec3(0, .5, 0));
      Model->rotate(propellerAngle, vec3(0, 1, 0));
      Model->scale(vec3(.3, .5, .3));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      // props accros
      Model->pushMatrix();
      Model->translate(vec3(0, .5, 0));
      Model->scale(vec3(4, .2, .5));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();
      Model->pushMatrix();
      Model->translate(vec3(0, .5, 0));
      Model->scale(vec3(.5, .2, 4));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();

      Model->popMatrix();
      Model->popMatrix();
      Model->pushMatrix();
      glUniform3fv(solidProg->getUniform("color"), 1,
                   glm::value_ptr(drone.droneColor));
      Model->translate(vec3(-.7, -.1, -.7));
      Model->scale(vec3(.8, .8, .8));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      // prop middle
      Model->pushMatrix();
      glUniform3f(solidProg->getUniform("color"), 1.0, 1.0, 1.0);
      Model->translate(vec3(0, .5, 0));
      Model->rotate(propellerAngle, vec3(0, 1, 0));
      Model->scale(vec3(.3, .5, .3));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      // props accros
      Model->pushMatrix();
      Model->translate(vec3(0, .5, 0));
      Model->scale(vec3(4, .2, .5));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();
      Model->pushMatrix();
      Model->translate(vec3(0, .5, 0));
      Model->scale(vec3(.5, .2, 4));
      setModel(solidProg, Model);
      cube->draw(solidProg);
      Model->popMatrix();

      Model->popMatrix();
      Model->popMatrix();
      Model->popMatrix();
      solidProg->unbind();
    }

    // animation update example
    sTheta = sin(glfwGetTime());
    cTheta = cos(glfwGetTime());
    eTheta = std::max(0.0f, (float)sin(glfwGetTime()));
    hTheta = std::max(0.0f, (float)cos(glfwGetTime()));
    // Pop matrix stacks.
    Projection->popMatrix();
    View->popMatrix();
  }

  void gameOver() {
    drone.chargeBattery();
    drone.endCombo();
    gameOverFlag = true;
  }

  // we need this to restart the game after user
  // gets gameOver screen
  void restartGame() {
    drone.reset();
    gameOverFlag = false;
  }

  void processKeyInput(GLFWwindow *window) {
    if (debugCam) {
      vec3 up = drone.orientation * vec3(0, 1, 0);
      vec3 front = drone.orientation * vec3(0, 0, -1);
      vec3 right = cross(up, front);
      float cameraSpeed = 20.0f * dt;
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        drone.position += front * cameraSpeed;
      }
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        drone.position += right * cameraSpeed;
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        drone.position -= right * cameraSpeed;
      }
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        drone.position -= front * cameraSpeed;
      }
    }
  }
};

int main(int argc, char *argv[]) {
  // Where the resources are loaded from
  std::string resourceDir = "../resources";

  if (argc >= 2) {
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
  while (!glfwWindowShouldClose(windowManager->getHandle())) {
    application->calculateDeltaTime();

    if (application->drone.battery <= 0) {
      application->gameOver();
    }
    // Render scene.
    application->render();
    application->processKeyInput(
        application->windowManager->getHandle()); // might change this to poll
    // we need to poll the input from gamepad
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
