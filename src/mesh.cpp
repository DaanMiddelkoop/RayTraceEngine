#include "mesh.h"

using namespace RT;

Mesh::Mesh(RayTraceContext* rtcontext)
{
    this->rtcontext = rtcontext;
}

void Mesh::setTriangles(uint8_t* triangles) {
    this->triangles = triangles;
}

void Mesh::update() {

}
