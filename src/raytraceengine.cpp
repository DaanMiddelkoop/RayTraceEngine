#include <iostream>

#include "raytraceengine.h"
#include "rtwindow.h"
#include "raytracecontext.h"

using namespace RT;

std::vector<RayTraceContext> rtcontexts;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void error_callback(int error, const char* description)
{
    std::cout << error << ", " << description << "\n";
}


int RT::init()
{
    std::cout << "Initing RT library v0.1\n";
    rtcontexts.reserve(10);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(error_callback);

    return 0;
}

Window* RT::createWindow()
{
    GLFWwindow* glwindow = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (glwindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(glwindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }

    Window* window = (Window*)std::malloc(sizeof(Window));
    window->glwindow = glwindow;

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(glwindow, framebuffer_size_callback);


    return window;
}

void RT::update(Window* window)
{
    glfwSwapBuffers(window->glwindow);
    glfwPollEvents();
}

int RT::windowShouldClose(Window* window)
{
    return glfwWindowShouldClose(window->glwindow);
}

RayTraceContext* RT::createWorld()
{
    RayTraceContext context;
    context.init();
    rtcontexts.push_back(context);

    return &rtcontexts.back();
}
