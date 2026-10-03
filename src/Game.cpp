#include "Game.h"

void Game::keyCallback(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {


  vec3 up = drone.orientation * vec3(0, 1, 0);
  vec3 front = drone.orientation * vec3(0, 0, -1);
  vec3 right = cross(up, front);
  float cameraSpeed = 2.5f * dt;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    goCamera_flag = !goCamera_flag;
  }

  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    drone.throttle = 1;
  }

  if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
    drone.throttle = 0;
  }

  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    debugCam_flag = !debugCam_flag;
  }

  if (key == GLFW_KEY_J && action == GLFW_PRESS) {
    joystick_flag = !joystick_flag;
  }

  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    if (gameOverFlag) {
      std::cout << "Restarting the game" << std::endl;
      restartGame();
    }
  }

  if (key == GLFW_KEY_O && action == GLFW_PRESS) {
    if (drone.getArmed()) {
      soundManager.stop(DRONE_PROPELLER);
    } else {
      soundManager.play(DRONE_PROPELLER);
    }
    drone.setArmed(!drone.getArmed());
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

void Game::mouseCallback(GLFWwindow *window, int button, int action, int mods) {
  double posX, posY;
  drone.getPosition();
  glfwGetCursorPos(window, &posX, &posY);
  std::cout << "Pos X " << posX << " Pos Y " << posY << std::endl;
}

// gather the deltaX and deltaY on scroll and change the phi and theta
// based off the sensitivity
void Game::scrollCallback(GLFWwindow *window, double deltaX, double deltaY) {
  if (debugCam_flag) {
    phi -= deltaY * camera_sensitivity;
    theta += deltaX * camera_sensitivity;
    drone.updateMouseOrientation(phi, theta, .005);
  }
}

// stold this from betaflight :p
float Game::get_rate(float stick_input, float rcRate, float superRate,
                     float baseDegPerSec) {
  float abs_input = fabs(stick_input);
  float base = stick_input * rcRate;
  float super = 1.0f / (1.0f - abs_input * superRate);
  float rate_deg = base * super * baseDegPerSec;
  float maxRate = rcRate * (1 / (1 - superRate)) * baseDegPerSec;
  // printf("%f\n", maxRate);
  return glm::radians(rate_deg);
}
// gather the controller inputs on callback
void Game::gamepadInputCallback(float leftX, float leftY, float rightX,
                                float rightY, bool left_bumper, bool gamepad) {
  gamepad_connected = gamepad;
  if (gamepad) {
    // left bumper pressed then cycle the arm state to either armed or
    // disarmed
    if (left_bumper) {
      if (drone.getArmed()) {
        soundManager.play(DRONE_DISARM);
        soundManager.stop(DRONE_PROPELLER);
      } else {
        soundManager.play(DRONE_PROPELLER);
        soundManager.play(DRONE_ARM);
      }

      drone.setArmed(!drone.getArmed());
    }
    // turn controller axie location into drone movement data
    drone.yawInput = -leftX;
    drone.pitchInput = rightY;
    drone.rollInput = rightX;
    right_joystick_overlay->updateJoystickPosition(-rightX, -rightY);
    left_joystick_overlay->updateJoystickPosition(leftX, leftY);
    drone.throttle = (leftY + 1) / 2; // clamp throttle [0, 1]
    // drone pitch
    soundManager.changeSoundPitch(DRONE_PROPELLER,
                                  max(drone.throttle + 0.5, 0.6));
  }
}

void Game::updateCamera(std::shared_ptr<MatrixStack> &view, Drone &drone) {
  vec3 direction = drone.orientation * vec3(0, 0, -1);
  vec3 eye = drone.position;
  vec3 up = drone.orientation * vec3(0, 1, 0);
  direction = glm::normalize(direction);
}

void Game::calculateDeltaTime() {
  using clock = std::chrono::high_resolution_clock;
  static auto lastTime = clock::now();

  auto currentTime = clock::now();
  std::chrono::duration<float> delta = currentTime - lastTime;
  lastTime = currentTime;

  dt = delta.count();
  dt = std::fmin(dt, 0.03);
}

void Game::updateUsingCameraPath(float frametime) {
  if (goCamera_flag || gameOverFlag) {
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
void Game::updateCamera(std::shared_ptr<MatrixStack> &view, vec3 drone_position,
                        quat drone_orientation, float drone_camera_angle) {
  // rotate around x axis to pitch
  quat cameraPitch = angleAxis(radians(drone_camera_angle), vec3(1, 0, 0));
  quat cameraOrientation = drone_orientation * cameraPitch;
  vec3 eye = drone_position;
  vec3 forward = cameraOrientation * vec3(0.0f, 0.0f, -1.0f);
  vec3 up = cameraOrientation * vec3(0.0f, 1.0f, 0.0f);
  // to where camera is
  if (goCamera_flag || gameOverFlag) {
    view->lookAt(gPos, gCenter, vec3(0, 1, 0));
  } else {
    view->lookAt(eye, eye + forward, up);
  }
}

void Game::resizeCallback(GLFWwindow *window, int width, int height) {
  camera_sensitivity = 180.0f / height;
  glViewport(0, 0, width, height);
  hud.setScreenSize(width, height);
}

void Game::init(const std::string &resourceDirectory) {

  GLSL::checkVersion();
  // Set background color.
  glClearColor(.72f, .84f, 1.06f, 1.0f);
  // Enable z-buffer test.
  glEnable(GL_DEPTH_TEST);

  /*--------------ENTITIES----------------*/

  lipo1 = std::make_shared<Lipo>(resourceDirectory);
  lipo2 = std::make_shared<Lipo>(resourceDirectory);
  lipo3 = std::make_shared<Lipo>(resourceDirectory);
  lipo4 = std::make_shared<Lipo>(resourceDirectory);
  lipo5 = std::make_shared<Lipo>(resourceDirectory);
  bonus1 = std::make_shared<Bonus>(resourceDirectory);
  bonus2 = std::make_shared<Bonus>(resourceDirectory);
  bonus3 = std::make_shared<Bonus>(resourceDirectory);
  special_trick_bonus1 = std::make_shared<SpecialTrickBonus>(resourceDirectory);
  special_trick_bonus2 = std::make_shared<SpecialTrickBonus>(resourceDirectory);
  entityProcess.add(lipo1);
  entityProcess.add(lipo2);
  entityProcess.add(lipo3);
  entityProcess.add(lipo4);
  entityProcess.add(lipo5);
  entityProcess.add(bonus1);
  entityProcess.add(bonus2);
  entityProcess.add(bonus3);
  entityProcess.add(special_trick_bonus1);
  entityProcess.add(special_trick_bonus2);

  /*--------------CAMERA PATHS-------------*/
  splinepath[0] =
      Spline(glm::vec3(-radius, 10, -radius), glm::vec3(-radius, 15, -radius),
             glm::vec3(radius, 15, -radius), glm::vec3(radius, 10, -radius), 5);
  splinepath[1] = Spline(glm::vec3(-45, 20, 10), glm::vec3(0), glm::vec3(0),
                         glm::vec3(-45, 20, -10), 10);
  splinepath[2] = Spline(glm::vec3(150, 10, 10), glm::vec3(150, 10, 10),
                         glm::vec3(150, 10, -20), glm::vec3(150, 10, -20), 10);

  /*----------------Progams-----------------*/
  // solid program for drawing solid colored objects
  solidProg = std::make_shared<Program>();
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
  textProg = std::make_shared<Program>();
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
  texProg = std::make_shared<Program>();
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

  // program for drawing skybox
  skyProg = std::make_shared<Program>();
  skyProg->setVerbose(true);
  skyProg->setShaderNames(resourceDirectory + "/shaders/skyVS.glsl",
                          resourceDirectory + "/shaders/skyFS.glsl");
  skyProg->init();
  skyProg->addUniform("P");
  skyProg->addUniform("V");
  skyProg->addUniform("skybox");
  skyProg->addAttribute("vertPos");

  /*------------Textures------------*/
  // texture for the style meter
  stylebar_sheet = std::make_shared<Texture>();
  stylebar_sheet->setFilename(resourceDirectory + "/stylebar_sheet.png");
  stylebar_sheet->init();
  stylebar_sheet->setUnit(1);
  stylebar_sheet->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  stylebar_sheet->setFiltering(GL_NEAREST, GL_NEAREST);

  /*----------Rendered Object--------*/
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

  left_joystick_overlay = std::make_shared<JoystickOverlay>();
  left_joystick_overlay->setResourceDir(resourceDirectory);
  left_joystick_overlay->init();
  left_joystick_overlay->setGlobalPosition(-6.25, -2.0f);

  right_joystick_overlay = std::make_shared<JoystickOverlay>();
  right_joystick_overlay->setResourceDir(resourceDirectory);
  right_joystick_overlay->init();
  right_joystick_overlay->setGlobalPosition(-5.25, -2.0f);

  soundManager.setResourceDir(resourceDirectory);
  soundManager.init();

  skybox.setFaces(faces);
  skybox.init();

  mountain_landscape.setResourceDir(resourceDirectory);
  mountain_landscape.init();

  ocean.setResourceDir(resourceDirectory);
  ocean.init();
  entityProcess.init();
}

void Game::initGeom(const std::string &resourceDirectory) {

  // we need to load the characters into the map datastructure for
  // text dispaly
  Text::load_characters(characters);

  // initialize the world bounding box
  worldBox.init();

  // load in the mesh and make the shape(s)
  std::vector<tinyobj::shape_t> TOshapesZ;
  std::vector<tinyobj::material_t> objMaterialsZ;
  std::string errStr;
  bool rc = tinyobj::LoadObj(TOshapesZ, objMaterialsZ, errStr,
                             (resourceDirectory + "/cube.obj").c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {
    cube = std::make_shared<Shape>();
    cube->createShape(TOshapesZ[0]);
    cube->measure();
    cube->init();
  }

  std::vector<tinyobj::shape_t> TOshapesB;
  std::vector<tinyobj::material_t> objMaterialsB;
  // load in the mesh and make the shape(s)
  rc =
      tinyobj::LoadObj(TOshapesB, objMaterialsB, errStr,
                       (resourceDirectory + "/mountain_landscape.obj").c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {

    mountain = std::make_shared<Shape>();
    mountain->createShape(TOshapesB[0]);
    mountain->measure();
    mountain->init();
  }
}

/*sets the program passed model uniform to the MatrixStack passed*/
void Game::setModel(std::shared_ptr<Program> prog,
                    std::shared_ptr<MatrixStack> M) {
  glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                     value_ptr(M->topMatrix()));
}

/*resizes the model into -1 to 1 range and centers at the origin*/
void Game::resize_and_center(vec3 gMin, vec3 gMax,
                             std::shared_ptr<MatrixStack> Model) {
  float center_x = (gMax.x + gMin.x) / 2;
  float center_y = (gMax.y + gMin.y) / 2;
  float center_z = (gMax.z + gMin.z) / 2;

  float largest_extent = std::max(
      std::max((gMax.x - gMin.x), (gMax.y - gMin.y)), (gMax.z - gMin.z));
  float scale = 2.0 / largest_extent;
  Model->translate(vec3(-center_x, -center_y, -center_z));
  Model->scale(vec3(scale, scale, scale));
}

// function to handle logic calls
void Game::handleLogic() {

  // update Drone sates
  float yawVel =
      get_rate(drone.yawInput, drone.rcRate, drone.superRate); // TODO move this
  float pitchVel = get_rate(drone.pitchInput, drone.rcRate, drone.superRate);
  float rollVel = get_rate(drone.rollInput, drone.rcRate, drone.superRate);

  // Update Camera Based on Flags
  if (goCamera_flag) {
    updateUsingCameraPath(dt);

  } else if (gameOverFlag) {
    updateUsingCameraPath(dt);
  } else {
    timeAlive += dt;
    if ((int)(timeAlive / 60) > difficultyLevel) {
      drone.increaseDifficulty(difficultyLevel);
      difficultyLevel = timeAlive / 60;
    }
    if (!debugCam_flag) {
      drone.updatePosition(dt);
    }
    drone.updateOrientation(rollVel, pitchVel, yawVel, dt);
    drone.updateTrickState(dt, soundManager);
    entityProcess.update(dt, drone, soundManager);
  }

  // TODO may be too weird passing sound manager to stuff that needs to
  // be played
  physicsWorld.handleDroneCollisions(drone, soundManager);
  Physics::clampToWorld(worldBox, drone);
  if (deathBox.intersects(drone.getAABB())) {
    gameOver();
  }
}

/*function to render the scene, dt is delta time*/
void Game::render() {
  // Get current frame buffer size.
  int width, height;
  glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
  glViewport(0, 0, width, height);

  // Clear framebuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  float aspect = width / (float)height;

  // Create the matrix stacks
  auto Projection = std::make_shared<MatrixStack>();
  auto View = std::make_shared<MatrixStack>();
  auto Model = std::make_shared<MatrixStack>();

  // Apply perspective projection.
  Projection->pushMatrix();
  Projection->perspective(glm::radians(75.0f), aspect, 0.01f, -400.0f);
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
  mountain_landscape.draw(texProg, Model, drone.position);
  // draw the entities
  entityProcess.draw(texProg, Model, drone);
  Model->popMatrix();
  texProg->unbind();
  /*all of the text*/
  textProg->bind();
  glUniform1i(textProg->getUniform("text"), 0);
  glUniformMatrix4fv(textProg->getUniform("P"), 1, GL_FALSE,
                     value_ptr(P_ortho));

  if (debugCam_flag) {
    Text::RenderText(textProg, "debug cam", 650, 550, .5, glm::vec3(1, 1, 1),
                     characters);
  }
  if (gameOverFlag) {

    Text::RenderText(textProg, std::string("Final Stats:"), 100, 550, .8f,
                     glm::vec3(1, 1, 1), characters);
    Text::RenderText(textProg,
                     std::string("Score................." +
                                 std::to_string(drone.finalScore)),
                     150, 500, .8f, glm::vec3(1, 1, 0), characters);
    std::ostringstream timeSurvivedOss;
    timeSurvivedOss << std::fixed << std::setprecision(0) << timeAlive / 60
                    << ":" << std::setw(2) << std::setfill('0')
                    << (int)timeAlive % 60;
    Text::RenderText(
        textProg, std::string("Time Alive............" + timeSurvivedOss.str()),
        150, 450, .8f, glm::vec3(1, 1, 0), characters);
    Text::RenderText(textProg,
                     std::string("Total Combos.........." +
                                 std::to_string(drone.totalCombos)),
                     150, 400, .8f, glm::vec3(1, 1, 0), characters);
    Text::RenderText(textProg,
                     std::string("Highest Combo........." +
                                 std::to_string(drone.highestCombo)),
                     150, 350, .8f, glm::vec3(1, 1, 0), characters);
    Text::RenderText(textProg,
                     std::string("Batteries Collected..." +
                                 std::to_string(drone.batteriesCollected)),
                     150, 300, .8f, glm::vec3(1, 1, 0), characters);
    Text::RenderText(textProg,
                     std::string("Obstacles Hit........." +
                                 std::to_string(drone.obstaclesHit)),
                     150, 250, .8f, glm::vec3(1, 1, 0), characters);

    Text::RenderText(textProg, "PRESS R TO TRY AGAIN", 400, 175,
                     .1 * sTheta + .7, glm::vec3(0, 1, 0), characters, 500,
                     true);
  }
  if (goCamera_flag) {
    // main menu
    Text::RenderText(textProg, "WHOOPS AND HOOPS", 300, 500, .1 * sTheta + 1,
                     glm::vec3(1, 1, 1), characters, 500, true);
    Text::RenderText(textProg, "Press G to start", 250, 100, .7,
                     glm::vec3(0, 1, 0), characters);
  } else if (!goCamera_flag && !gameOverFlag && hud_flag) {
    // main hud
    int speed = static_cast<int>(length(drone.velocity));
    glm::vec3 batLevelColor{};

    // render information in bottom left
    Text::RenderText(textProg, std::string("SPEED: " + std::to_string(speed)),
                     25.0f, 25.0f, .75f, glm::vec3(0.5, 0.8f, 0.2f),
                     characters);
    Text::RenderText(textProg, "ACRO", 25.0f, 75.0f, .75f,
                     glm::vec3(0.5, 0.8f, 0.2f), characters);

    std::ostringstream timerOss;
    timerOss << std::fixed << std::setprecision(0) << (int)(timeAlive / 60)
             << ":" << std::setw(2) << std::setfill('0') << (int)timeAlive % 60;
    // time alive trakcer
    Text::RenderText(textProg, timerOss.str(), 650.0f, 550.0f, .75f,
                     glm::vec3(1.f, 1.f, 1.f), characters);
    // battery level
    if (drone.battery >= 75)
      batLevelColor = {0.5, 0.8f, 0.2f};
    else if (drone.battery >= 50)
      batLevelColor = {1.0f, 0.8f, 0.0f};
    else if (drone.battery >= 0)
      batLevelColor = {1.0f, 0.0f, 0.0f};
    Text::RenderText(
        textProg,
        std::string("BAT: " + std::to_string(static_cast<int>(drone.battery))),
        25.0f, 125.0f, .75f, batLevelColor, characters);

    // render score & trick description
    if (drone.trickCount > 0) {
      textFallY = 0.0;
      Text::RenderText(textProg,
                       std::string(std::to_string(drone.score) + " x " +
                                   std::to_string(drone.trickCount)),
                       340.0f + sin(glfwGetTime() * drone.trickCount) * .5,
                       70.0f + cos(glfwGetTime() * drone.trickCount) * .5, .8f,
                       glm::vec3(1, 1, 1), characters);

      Text::RenderText(textProg, drone.trick, 400.0f, 50.0f, .5f,
                       glm::vec3(1, 1, 0), characters, 500, true);

    } else if (drone.oldTrick != "") {
      textFallY += text_fallSpeed * dt;
      Text::RenderText(textProg,
                       std::string(std::to_string(drone.oldScore) + " x " +
                                   std::to_string(drone.trickCount)),
                       340.0f, 70.0f - textFallY, .8f, glm::vec3(1, 0, 0),
                       characters);
      // render drone trick description
      Text::RenderText(textProg, drone.oldTrick, 400.0f, 50.0f - textFallY, .5f,
                       glm::vec3(1, 0, 0), characters, 500, true);
    }

    Text::RenderText(
        textProg, std::string("Score: " + std::to_string(drone.totalScore)),
        255.0f, 550.0f, .5f, glm::vec3(0, 0, 0), characters, 500, true);
  }
  if (!gamepad_connected) {
    // gamepad disconnnected
    if (!gamepad_connected) {
      Text::RenderText(textProg, "NO GAMEPAD DETECTED!", 225, 50, .7,
                       glm::vec3(1, 0, 0), characters);
    }
  }
  textProg->unbind();

  // draw the joystick overlay

  // draw and update hud
  if (!goCamera_flag && !gameOverFlag) {
    if (joystick_flag) {
      right_joystick_overlay->draw();
      left_joystick_overlay->draw();
    }

    glDisable(GL_BLEND);
    if (hud_flag) {

      float fill = drone.styleScore / drone.special_score_thresh - dt;
      hud.setTargetFill(fill);
      hud.update(dt);
      hud.draw();
      if (drone.special_mode) {
        style_meter.size = glm::vec2(sTheta, cTheta);
      }
    }
  }
  glClear(GL_DEPTH_BUFFER_BIT);

  // draw the drone
  if (!goCamera_flag && !gameOverFlag && hud_flag) {
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

    if (drone.health >= 4) {
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
    }

    Model->popMatrix();
    Model->pushMatrix();
    glUniform3fv(solidProg->getUniform("color"), 1,
                 glm::value_ptr(drone.droneColor));
    Model->translate(vec3(-.7, -.1, .7));
    Model->scale(vec3(.8, .8, .8));
    setModel(solidProg, Model);
    cube->draw(solidProg);

    if (drone.getHealth() >= 3) {
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
    }
    Model->popMatrix();
    Model->pushMatrix();
    glUniform3fv(solidProg->getUniform("color"), 1,
                 glm::value_ptr(drone.droneColor));
    Model->translate(vec3(.7, -.1, -.7));
    Model->scale(vec3(.8, .8, .8));
    setModel(solidProg, Model);
    cube->draw(solidProg);

    if (drone.getHealth() >= 2) {
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
    }
    Model->popMatrix();
    Model->pushMatrix();
    glUniform3fv(solidProg->getUniform("color"), 1,
                 glm::value_ptr(drone.droneColor));
    Model->translate(vec3(-.7, -.1, -.7));
    Model->scale(vec3(.8, .8, .8));
    setModel(solidProg, Model);
    cube->draw(solidProg);
    // prop middle
    if (drone.getHealth() >= 1) {

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
    }
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

void Game::gameOver() {
  drone.chargeBattery();
  drone.endCombo();
  drone.setArmed(false);
  soundManager.stop(DRONE_PROPELLER);
  gameOverFlag = true;
  difficultyLevel = 0;
}

// we need this to restart the game after user
// gets gameOver screen
void Game::restartGame() {
  timeAlive = 0;
  drone.reset();
  gameOverFlag = false;
}

void Game::processKeyInput(GLFWwindow *window) {
  if (debugCam_flag) {
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

Drone *Game::getDrone() { return &drone; }
