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
        void build();
        void update();
        void setMaterial(MaterialHandle* material);

    protected:

    private:
        // Location in the rtcontext triangle of where our block starts.
        int tbegin;
        int tend;

        int material_id;

        Tree* aabbtree;

        RayTraceContext* rtcontext;

};
}
#endif // MESH_H
