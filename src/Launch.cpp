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

unsigned int buffer;
unsigned int indexBuffer;

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

    // Set framerate
    glfwSwapInterval(10);

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

static void createVertexShader()
{
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        int length;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(vertexShader, length, &length, message);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << message << std::endl;
    }
}

static void createFragmentShader()
{
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    int success;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        int length;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(fragmentShader, length, &length, message);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << message << std::endl;
    }
}

static void linkShaders()
{
    // link shaders to a program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    // check for linking errors
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) 
    {
        int length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(shaderProgram, length, &length, message);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << message << std::endl;
    }
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
    linkShaders();

    // Delete Shader objects as we've compiled and linked the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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

    // Vertex Buffer for draw call
    float positions[] =
    {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    unsigned int indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex Buffer
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // Index Buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Create Shaders and link to a program created on the GPU
    createShaders();

    glUseProgram(shaderProgram);

    int location = glGetUniformLocation(shaderProgram, "u_Color");
    glUniform4f(location, 0.0f, 1.0f, 0.0f, 1.0f);

    float r = 0.0f;
    float increment = 0.05f;
    // Render Loop, will loop through until the window is closed
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // Draw code
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform4f(location, r, 0.0f, 1.0f, 1.0f);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        if (r > 1.0f)
        {
            increment = -0.05f;
        }
        else if (r < 0.0f)
        {
            increment = 0.05f;
        }

        r += increment;

        // Swap the buffers
        glfwSwapBuffers(window);

        // Poll and process events
        glfwPollEvents();
    }

    cleanProgram();

	return EXIT_SUCCESS;
}