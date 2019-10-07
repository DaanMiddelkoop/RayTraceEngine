#ifndef RTWINDOW_H_INCLUDED
#define RTWINDOW_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace RT
{
class Window
{

public:
    GLFWwindow* glwindow;
};
}

#endif // RTWINDOW_H_INCLUDED
