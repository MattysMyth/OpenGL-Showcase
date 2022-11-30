#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <fstream>

std::string vertexShaderString;
std::string fragmentShaderString;

const char* vertexShaderSource;
const char* fragmentShaderSource;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

// Screen sizes, default values set at 800 x 600
unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;

bool windowedMode = true;

GLFWwindow* window;

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

static bool initGLFW()
{
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
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids", monitorWindow, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Set what function to call when the window is resized, passed as a parameter
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return true;
}

static bool initGLAD()
{
    // Initialise GLAD, throw an error if GLAD failed to initialise
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    return true;
}

static void cleanProgram()
{
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
}

static void createShaders()
{
    // Build and compile a basic vertex shader
    vertexShaderString = parseShader("src/shaders/BasicVertex.shader");
    vertexShaderSource = vertexShaderString.c_str();
    createVertexShader();

    // Build and compile a basic fragment shader
    fragmentShaderString = parseShader("src/shaders/BasicFragment.shader");
    fragmentShaderSource = fragmentShaderString.c_str();
    createFragmentShader();

    // Create a program and link shaders for use
    linkShaders(vertexShader, fragmentShader);

    // Delete Shader objects as we've compiled and linked the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

static void createVertexShader()
{
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource , NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

static void createFragmentShader()
{
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

static void linkShaders(unsigned int vertexShader, unsigned int fragmentShader)
{
    // link shaders to a program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

int main()
{
    // Initialize GLFW
    if (!initGLFW())
    {
        return -1;
    }

    // Initialize GLAD
    if (!initGLAD())
    {
        return -1;
    }

    // Tells OpenGL the size of the viewport, used as reference for OpenGL functions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Create Shaders and link to a program created on the GPU
    createShaders();

    // Render Loop, will loop through until the window is closed
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanProgram();

	return EXIT_SUCCESS;
}