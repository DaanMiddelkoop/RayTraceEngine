#ifndef MESH_H
#define MESH_H

#include <vector>
#include <stdint.h>
#include "raytracecontext.h"
#include "tree.h"

namespace RT
{
class Mesh
{
    public:
        Mesh(RayTraceContext* rtcontext);
        void setVertices(float* vertices, int length);
        void setTriangles(std::vector<Triangle>* triangles);
        void setUVCoords(float* coords, int length);
        void build();
        void update();
        void setMaterial(MaterialHandle* material);
        int createRootNode();

        void setPosition(float x, float y, float z);
        void setRotation(float x, float y, float z);

        void move(float x, float y, float z);
        void rotate(float x, float y, float z);

        Matrix4x4* getTransform();


        int rootIndex = -1;

    protected:

    private:
        void updateRootTransform();

        // Location in the rtcontext triangle of where our block starts.
        int tbegin;
        int tend;


        int material_id;

        Tree* aabbtree;

        RayTraceContext* rtcontext;

        int transform_id;

};
}
#endif // MESH_H
