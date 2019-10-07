#ifndef WORLD_H
#define WORLD_H

#include "rtwindow.h"
#include <vector>

namespace RT {
    class Mesh;

    class RayTraceContext
    {
        public:
            RayTraceContext();
            void init();

            Mesh* createMesh();
            void draw(RT::Window* window);

        protected:

        private:
            std::vector<Mesh> meshes;
            unsigned int shaderProgram;
            unsigned int VBO, VAO;
    };
}

#endif // WORLD_H
