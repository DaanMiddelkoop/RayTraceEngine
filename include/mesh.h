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

        void setPosition(float x, float y, float z);
        void setRotation(float x, float y, float z);

        void move(float x, float y, float z);
        void rotate(float x, float y, float z);



    protected:

    private:
        void updateRootTransform();

        // Location in the rtcontext triangle of where our block starts.
        int tbegin;
        int tend;

        int rootIndex = -1;

        int material_id;

        Tree* aabbtree;

        RayTraceContext* rtcontext;

        Matrix4x4 transform;

};
}
#endif // MESH_H
