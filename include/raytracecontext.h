#ifndef WORLD_H
#define WORLD_H

#include "rtwindow.h"
#include <vector>
#include "Triangle.h"
#include "tree.h"
#include "texture.h"
#include "material.h"

namespace RT {
    class Mesh;
    class Material;
    class MaterialHandle;
    class TextureHandle;
    class Texture;

    class RayTraceContext
    {
        public:
            RayTraceContext();
            void init();

            // Create a mesh in the scene and give a reference to user.
            Mesh* createMesh();

            // Create a new material in the scene and give a reference to user;
            MaterialHandle createMaterial();

            // Retrueve reference to raw material data;
            Material* getMaterial(int material_id);

            // Create a new texture.
            TextureHandle createTexture();

            // Retrieve reference to texture data;
            Texture* getTexture(int texture_id);

            // Retrieve reference to raw pixel data;
            std::vector<unsigned int>* getPixels();


            void draw(RT::Window* window);

            // Returns the index at which it started placing triangles.
            int addTriangles(std::vector<Triangle>* triangles);

            // Removes triangles from the list to save memory.
            void removeTriangles(int tbegin, int tend);

            // Add a tree of nodes to the scene tree and return the location of the sub tree.
//            int addNode(std::vector<Tree>* nodes);

            std::vector<Tree>* getNodes();

            std::vector<Triangle>*  getTriangles();

            int addTransform(Matrix4x4* transform);

            std::vector<Matrix4x4>* getTransforms();



            // Refresh the triangles to gpu in case of resize.
            void updateGPUTriangles();

            // Refresh the treenodes to gpu in case of resize.
            void updateGPUTreenodes();

            // Update gpu materials.
            void updateGPUMaterials();

            // Refresh all textures on the gpu.
            void updateGPUTextures();

            // Refresh all pixels of textures on the gpu.
            void updateGPUPixels();

            void updateGPUTransforms();

            void updateGPUTransform(int transform_id);

            // Update part of triangles on gpu
            void updateGPUTrianglesPartial(int b, int e);

            // Update part of treenodes on gpu.
            void updateGPUTreenodesPartial(int b, int e);

            void updateGPUTreenodesPath(int node);


            // Set the position of the camera in the scene.
            void setCameraPosition(float x, float y, float z);

            // Set the direction of the camera in the scene.
            void setCameraDirection(float x, float y, float z);

            void setDebugInfo(float x, float y, float z);

            int recoverSceneRoot();



        protected:

        private:
            std::vector<Mesh*> meshes;
            std::vector<Triangle> triangles;
            std::vector<Tree> aabbtree;
            std::vector<Material> materials;
            std::vector<Texture> textures;
            std::vector<unsigned int> pixels;
            std::vector<Matrix4x4> transforms;

            unsigned int shaderProgram;
            unsigned int VBO, VAO;

            GLuint triangle_ssbo;
            GLuint treenode_ssbo;
            GLuint materials_ssbo;
            GLuint textures_ssbo;
            GLuint pixels_ssbo;
            GLuint transforms_ssbo;

            GLuint eye;
            GLuint eye_dir;

            GLuint screen_size;
            GLuint debug_info;

            GLuint scene_root;

            void setSceneRoot(int root);
    };
}

#endif // WORLD_H
