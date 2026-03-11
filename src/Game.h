#ifndef GAME_H
#define GAME_H
#include "AABB.h"
#include "Bonus.h"
#include "JoystickOverlay.h"
#include "Drone.h"
#include "EntityProcess.h"
#include "GLSL.h"
#include "Hud.h"
#include <sstream>
#include <iomanip>
#include "Lipo.h"
#include "MatrixStack.h"
#include "Mountain.h"
#include "OBB.h"
#include "Physics.h"
#include "PhysicsWorld.h"
#include "Program.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Shape.h"
#include "SoundManager.h"
#include "Spline.h"
#include "Text.h"
#include "Texture.h"
#include "WindowManager.h"
#include "ocean.h"
#include "skybox.h"
#include <chrono>
#include <glad/glad.h>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

#define PI 3.1415926535

//TODO #define LEFT_JOYSTICK_POSITION glm::vec2()
//TODO #define RIGHT_JOYSTICK_POSITION glm::vec2()

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Game : public EventCallbacks {
public:
  WindowManager *windowManager = nullptr;
  void init(const std::string &resourceDirectory);
  void initGeom(const std::string &resourceDirectory);
  void calculateDeltaTime();
  void gameOver();
  void render();
  void handleLogic();
  void processKeyInput(GLFWwindow *window);
  Drone *getDrone();

private:
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
  Mountain mountain_landscape;
  // scene stuff
  Scene scene;
  ResourceManager resourceManager;
  PhysicsWorld physicsWorld;
  SoundManager soundManager;
  // render class
  // Our shader programs
  std::shared_ptr<Program> textProg;
  std::shared_ptr<Program> texProg;
  std::shared_ptr<Program> solidProg;
  std::shared_ptr<Program> skyProg;
  std::shared_ptr<Program> billboardProg;
  // our static geometry
  std::shared_ptr<Shape> cube;
  std::shared_ptr<Shape> mountain;
  // the image to use as a texture (ground)
  std::shared_ptr<Texture> stylebar_sheet;
  std::map<char, Character> characters;
  float dt;
  // Handles operating on entities
  EntityProcess entityProcess;
  // lipo batteries
  std::shared_ptr<Lipo> lipo1;
  std::shared_ptr<Lipo> lipo2;
  std::shared_ptr<Lipo> lipo3;
  std::shared_ptr<Lipo> lipo4;
  std::shared_ptr<Lipo> lipo5;
  // Bonus pickups
  std::shared_ptr<Bonus> bonus1;
  std::shared_ptr<Bonus> bonus2;
  std::shared_ptr<Bonus> bonus3;
  // variables used for camera positing
  vec3 gPos; // global Pos
  vec3 gCenter = vec3(0, 0, 0);
  float radius = 100;
  float phi = 0.0f;
  float elapsedTime = 0.0f;
  float theta = PI / 2;
  float camera_sensitivity = .1;
  // gamepad
  bool gamepad_connected = false;
  float yawDelta = 0;
  float pitchDelta = 0;
  float rollDelta = 0;
	std::shared_ptr<JoystickOverlay> left_joystick_overlay;
	std::shared_ptr<JoystickOverlay> right_joystick_overlay;
  // animation data
  float gTrans = -3;
  float sTheta = 0;
  float cTheta = 0;
  float eTheta = 0;
  float hTheta = 0;
  float textFallY = 0.0f;
  float text_fallSpeed = 30.0f;
  bool debugCam_flag = false;
  bool hud_flag = true;
  bool goCamera_flag = true;
  bool gameOverFlag = false;
	float timeAlive = 0.0f;
	int difficultyLevel = 0;
  // camera spline animation
  Spline splinepath[3];
  int currentSpline = 0;
  int numSplines = 3;
	

  Drone drone;
  AABB worldBox = AABB(vec3(-170, -20, -170), vec3(170, 200, 250));

  void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                   int mods);
  void mouseCallback(GLFWwindow *window, int button, int action, int mods);
  void scrollCallback(GLFWwindow *window, double deltaX, double deltaY);
  void gamepadInputCallback(float leftX, float leftY, float rightX,
                            float rightY, bool left_bumper, bool gamepad);
  void resizeCallback(GLFWwindow *window, int width, int height);
  float get_rate(float stick_input, float rcRate, float superRate,
                 float baseDegPerSec = 200.0f);
  void updateCamera(std::shared_ptr<MatrixStack> &view, Drone &drone);
  void updateUsingCameraPath(float frametime);
  void updateCamera(std::shared_ptr<MatrixStack> &view, vec3 drone_position,
                    quat drone_orientation, float drone_camera_angle);
  void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M);
  void resize_and_center(vec3 gMin, vec3 gMax,
                         std::shared_ptr<MatrixStack> Model);
  void restartGame();
};

#endif
