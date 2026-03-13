#include "WindowManager.h"
#include "Game.h"
#include <iostream>
int main(int argc, char *argv[]) {
  // Where the resources are loaded from
  std::string resourceDir = "../resources";
  if (argc >= 2) {
    resourceDir = argv[1];
  }
  Game *game = new Game();
  // Your main will always include a similar set up to establish your window
  // and GL context, etc.
  WindowManager *windowManager = new WindowManager();
  windowManager->init(640, 480);
  windowManager->setEventCallbacks(game);
  game->windowManager = windowManager;
  // This is the code that will likely change program to program as you
  // may need to initialize or set up different data and state
  game->init(resourceDir);
  game->initGeom(resourceDir);
  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(windowManager->getHandle())) {
    game->calculateDeltaTime();
    if (game->getDrone()->battery <= 0 || game->getDrone()->health <= 0) {
      game->gameOver();
    }
    // Render scene.
		game->handleLogic();
    game->render();
    game->processKeyInput(
        game->windowManager->getHandle()); // might change this to poll
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
