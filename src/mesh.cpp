#include <iostream>
#include "mesh.h"
#include <vector>

using namespace RT;

Mesh::Mesh(RayTraceContext* rtcontext)
{
    this->rtcontext = rtcontext;
}

void Mesh::setVertices(float* vertices, int length) {
    // Contruct mesh from raw vertices.

    std::cout << "TEST!123" << vertices[0] << "\n";
    rtcontext->removeTriangles(tbegin, tend);


    //rtcontexts->addVertices(vertices, length);
    std::vector<Triangle> ts;
    for (int i = 0; i < length / 9; i++) {
        float* vs = vertices + (i * 9);
        Triangle t = Triangle();
        t.x1 = vs[0];
        t.y1 = vs[1];
        t.z1 = vs[2];
        t.x2 = vs[3];
        t.y2 = vs[4];
        t.z2 = vs[5];
        t.x3 = vs[6];
        t.y3 = vs[7];
        t.z3 = vs[8];


        ts.push_back(t);
    }

    setTriangles(&ts);
}

void Mesh::setTriangles(std::vector<Triangle>* triangles) {

    std::cout << "setTriangles size: " << triangles->size() << "\n";
    this->tbegin = rtcontext->addTriangles(triangles);
    this->tend = tbegin + triangles->size();

}

void Mesh::build() {
    // Remove all of the

    std::vector<Tree> nodes = std::vector<Tree>(0);
    nodes.reserve((tend - tbegin) * 2);


    Tree root = Tree();
    root.setBoundaries(&rtcontext->getTriangles()->at(0));
    nodes.push_back(root);
    for (int i = tbegin; i < tend; i++) {
        Tree node = Tree();
        node.leaf_id = i;
        node.setBoundaries(&rtcontext->getTriangles()->at(i));
        std::cout << "area: " << node.getArea() << "\n";



        nodes[0].insertNode(&nodes, 0, node);

    }

    std::cout << "printing tree stuff\n";
    for (unsigned int a = 0; a < nodes.size(); a++) {
        std::cout << a << ", " << nodes[a].node1 << ", " << nodes[a].node2 << ", " << nodes[a].leaf << ", " << nodes[a].getArea() * 1000000.f << "\n";
    }


    rtcontext->addNode(&nodes);
}

void Mesh::update() {
    // Balance tree better
}
