#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>

// Screen sizes, default values set at 800 x 600
unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;

bool windowedMode = false;

std::string vertexShaderSource;
std::string fragmentShaderSource;

// Sets glViewport with given width and height. Used as a function to be called on resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Processes keyboard input via glfw
void processInput(GLFWwindow* window)
{
	// If the escape key is pressed, flag glfw to close the window and escape the render loop
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

static std::string parseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    std::stringstream ss;
    std::string line;
    while (std::getline(stream, line))
    {
        ss << line << '\n';
    }

    return ss.str();
}

int main()
{
    // -------------------------GLFW Init -----------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitorWindow = NULL;
    if (!windowedMode)
    {
        monitorWindow = glfwGetPrimaryMonitor();
        const GLFWvidmode* monitorMode = glfwGetVideoMode(monitorWindow);
        SCREEN_WIDTH = monitorMode->width;
        SCREEN_HEIGHT = monitorMode->height;
    }

    // Initialise a window, throw an error if window failed to initialise
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids", monitorWindow, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set what function to call when the window is resized, passed as a parameter
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ---------------------GLFW init end-----------------------------

        // Initialise GLAD, throw an error if GLAD failed to initialise
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Tells OpenGL the size of the viewport, used as reference for OpenGL functions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Render Loop, will loop through until the window is closed
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

	return EXIT_SUCCESS;
}