
#include "WindowManager.h"
#include "GLSL.h"

#include <iostream>


void error_callback(int error, const char *description)
{
	std::cerr << description << std::endl;
}

WindowManager * WindowManager::instance = nullptr;

WindowManager::WindowManager()
{
	if (instance)
	{
		std::cerr << "One instance of WindowManager has already been created, event callbacks of new instance will not work." << std::endl;
	}

	instance = this;
}

WindowManager::~WindowManager()
{
	if (instance == this)
	{
		instance = nullptr;
	}
}

bool WindowManager::init(int const width, int const height)
{
	glfwSetErrorCallback(error_callback);

	// Initialize glfw library
	if (!glfwInit())
	{
		return false;
	}

	//request the highest possible version of OGL - important for mac
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	windowHandle = glfwCreateWindow(width, height, "Whoops and Hoops", nullptr, nullptr);
	if (! windowHandle)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(windowHandle);

	// Initialize GLAD
	if (!gladLoadGL())
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// Set vsync
	glfwSwapInterval(1);

	glfwSetKeyCallback(windowHandle, key_callback);
	glfwSetMouseButtonCallback(windowHandle, mouse_callback);
	glfwSetFramebufferSizeCallback(windowHandle, resize_callback);
	glfwSetScrollCallback(windowHandle, scroll_callback);

	return true;
}

void WindowManager::shutdown()
{
	glfwDestroyWindow(windowHandle);
	glfwTerminate();
}

void WindowManager::setEventCallbacks(EventCallbacks * callbacks_in)
{
	callbacks = callbacks_in;
}

GLFWwindow * WindowManager::getHandle()
{
	return windowHandle;
}

void WindowManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->keyCallback(window, key, scancode, action, mods);
	}
}

void WindowManager::mouse_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->mouseCallback(window, button, action, mods);
	}
}

void WindowManager::resize_callback(GLFWwindow * window, int in_width, int in_height)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->resizeCallback(window, in_width, in_height);
	}
}

void WindowManager::scroll_callback(GLFWwindow * window, double in_deltaX, double in_deltaY)
{
	if (instance && instance->callbacks)
	{
		instance->callbacks->scrollCallback(window, in_deltaX, in_deltaY);
	}
}

//poll gamepad input for gamepad or joystick(transmitter)
void WindowManager::pollGamepadInput(){
    int joystickID = GLFW_JOYSTICK_1;
    if(glfwJoystickPresent(GLFW_JOYSTICK_1)){
        //gamepad(xbox controller)
        if(glfwJoystickIsGamepad(joystickID)){
            GLFWgamepadstate state;  
            if(glfwGetGamepadState(GLFW_JOYSTICK_1, &state)){
                float leftX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
                float leftY = -state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

                float rightX= -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
                float rightY= state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
                if (instance && instance->callbacks) {
                    instance->callbacks->gamepadInputCallback(leftX, leftY, rightX, rightY, true);
                }

            }
        }
        //other(transmitter)
        else{
            int count;
            const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

            if(count == 0 || !axes){
                std::cerr << "Error joystick detected, axes not detected!" << std::endl;
                return;
            }
            float rightX = -axes[0];
            float rightY = -axes[1];

            float leftX = axes[3];
            float leftY = axes[2];


            if(instance && instance->callbacks){
                instance->callbacks->gamepadInputCallback(leftX, leftY, rightX, rightY, true);
            }
        }
    }else{
        instance->callbacks->gamepadInputCallback(0, 0, 0, 0, false);
    }


}

