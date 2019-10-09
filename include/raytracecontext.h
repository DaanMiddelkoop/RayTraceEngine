#ifndef WORLD_H
#define WORLD_H

#include "rtwindow.h"
#include <vector>
#include "Triangle.h"
#include "tree.h"

namespace RT {
    class Mesh;

    class RayTraceContext
    {
        public:
            RayTraceContext();
            void init();

            Mesh* createMesh();
            void draw(RT::Window* window);

            // Returns the index at which it started placing triangles.
            int addTriangles(std::vector<Triangle>* triangles);

            // Removes triangles from the list to save memory.
            void removeTriangles(int tbegin, int tend);

            // Add a tree of nodes to the scene tree and return the location of the sub tree.
            int addNode(std::vector<Tree>* nodes);

            std::vector<Triangle>*  getTriangles();

            // Refresh this part of the triangles to the gpu.
            void updateGPUTriangles(int b, int e);

            // Refresh this part of the aabb tree to the gpu.
            void updateGPUTreenodes(int b, int e);


        protected:

        private:
            std::vector<Mesh> meshes;
            std::vector<Triangle> triangles;
            std::vector<Tree> aabbtree;
            unsigned int shaderProgram;
            unsigned int VBO, VAO;

            GLuint triangle_ssbo;
            GLuint treenode_ssbo;
    };
}

#endif // WORLD_H
