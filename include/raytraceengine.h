#ifndef RAYTRACEENGINE_H_INCLUDED
#define RAYTRACEENGINE_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "raytracecontext.h"
#include "rtwindow.h"
#include "mesh.h"
#include "objloader.h"

namespace RT
{
int init();
int windowShouldClose(Window* window);
void update(Window* window);
Window* createWindow();
RayTraceContext* createWorld();
}

#endif // RAYTRACEENGINE_H_INCLUDED
