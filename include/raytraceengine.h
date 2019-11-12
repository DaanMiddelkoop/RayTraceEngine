#ifndef RAYTRACEENGINE_H_INCLUDED
#define RAYTRACEENGINE_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "rtwindow.h"
#include "raytracecontext.h"
#include "mesh.h"
#include "objloader.h"
#include "treevisualiser.h"

namespace RT
{
int init();
int windowShouldClose(Window* window);
void update(Window* window);
Window* createWindow();
RayTraceContext* createWorld();
void setKeyCallBack(Window* window, void (*) (GLFWwindow*, int, int, int, int));
}

#endif // RAYTRACEENGINE_H_INCLUDED
