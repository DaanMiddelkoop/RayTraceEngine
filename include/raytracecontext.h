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

            // Refresh the triangles to gpu in case of resize.
            void updateGPUTriangles();

            // Refresh the treenodes to gpu in case of resize.
            void updateGPUTreenodes();

            // Update part of triangles on gpu
            void updateGPUTrianglesPartial(int b, int e);

            // Update part of treenodes on gpu.
            void updateGPUTreenodesPartial(int b, int e);

            // Set the position of the camera in the scene.
            void setCameraPosition(float x, float y, float z);

            // Set the direction of the camera in the scene.
            void setCameraDirection(float x, float y, float z);

            void setDebugInfo(float x, float y, float z);



        protected:

        private:
            std::vector<Mesh> meshes;
            std::vector<Triangle> triangles;
            std::vector<Tree> aabbtree;
            unsigned int shaderProgram;
            unsigned int VBO, VAO;

            GLuint triangle_ssbo;
            GLuint treenode_ssbo;

            GLuint eye;
            GLuint eye_dir;

            GLuint screen_size;
            GLuint debug_info;
    };
}

#endif // WORLD_H
