#ifndef MESH_H
#define MESH_H

#include <vector>
#include <stdint.h>
#include "raytracecontext.h"

namespace RT
{
class Mesh
{
    public:
        Mesh(RayTraceContext* rtcontext);
        void setTriangles(uint8_t* triangles);
        void update();

    protected:

    private:
        uint8_t* triangles;
        RayTraceContext* rtcontext;

};
}
#endif // MESH_H
