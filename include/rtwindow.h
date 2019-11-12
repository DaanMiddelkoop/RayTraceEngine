#ifndef RTWINDOW_H_INCLUDED
#define RTWINDOW_H_INCLUDED

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#warning "TEST";

namespace RT
{
    class Window
    {

    public:
        GLFWwindow* glwindow;

        Window();

        void setbladiebla(int test);
    };

    class Window2
    {
    };
}




#endif // RTWINDOW_H_INCLUDED
