#include <iostream>
#include "mesh.h"
#include <vector>

using namespace RT;

Mesh::Mesh(RayTraceContext* rtcontext)
{
    this->rtcontext = rtcontext;
    this->material_id = -1;

    Matrix4x4 transform = Matrix4x4();
    this->transform_id = rtcontext->addTransform(&transform);
    rtcontext->updateGPUTransforms();

    std::cout << "New mesh created and assigned a transform id: " << transform_id << std::endl;
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

void Mesh::setUVCoords(float* coord, int length) {
    for (int i = 0; i < length; i += 6) {
        Triangle* t = &rtcontext->getTriangles()->at(tbegin + (i / 6));

        t->u1 = coord[i];
        t->v1 = coord[i + 1];
        t->u2 = coord[i + 2];
        t->v2 = coord[i + 3];
        t->u3 = coord[i + 4];
        t->v3 = coord[i + 5];
    }
}

void Mesh::build() {
    // Remove all of the


    std::vector<Tree> nodes = std::vector<Tree>(0);
    nodes.reserve((tend - tbegin) * 5);


    Tree root = Tree();
    root.setBoundaries(&rtcontext->getTriangles()->at(0));
    nodes.push_back(root);
    for (int i = tbegin; i < tend; i++) {
        if (i % 10000 == 0) {
            std::cout << "Insert node " << i << " out of " << (tend - tbegin) << "\n";
        }

        // Update triangle material.
        rtcontext->getTriangles()->at(i).material = material_id;

        Tree node = Tree();
        node.leaf_id = i;
        node.setBoundaries(&rtcontext->getTriangles()->at(i));



        nodes[0].insertNode(&nodes, 0, node);

    }

    nodes[0].isObject = true;

    this->rootIndex = rtcontext->addNode(&nodes);
}

void Mesh::update() {
    // Balance tree better
}

void Mesh::setMaterial(MaterialHandle* material) {
    this->material_id = material->material_id;
}

void Mesh::setPosition(float x, float y, float z) {
    this->getTransform()->setPosition(x, y, z);
    updateRootTransform();
}

void Mesh::move(float x, float y, float z) {
    this->getTransform()->move(x, y, z);
    updateRootTransform();
}

void Mesh::setRotation(float x, float y, float z) {
    this->getTransform()->setRotation(x, y, z);
    updateRootTransform();
}

void Mesh::rotate(float x, float y, float z) {
    this->getTransform()->rotate(x, y, z);
    updateRootTransform();
}

void Mesh::updateRootTransform() {
    if (rootIndex != -1) {


        int root_transform_id = (*rtcontext->getNodes())[rootIndex].transform_id;

        if (root_transform_id != transform_id) {

            std::cout << root_transform_id << " , " << transform_id << " , " << rootIndex <<  std::endl;
            std::cout << "Setting transform_parents of object " << this << std::endl;
            (*rtcontext->getNodes())[rootIndex].transform_id = transform_id;
            (*rtcontext->getNodes())[rootIndex].updateTransformParents(rtcontext->getNodes());
            rtcontext->updateGPUTreenodes();
        }

        rtcontext->getNodes()->at(rootIndex).updateTransformBoundingBox(rtcontext->getNodes(), getTransform());

        rtcontext->updateGPUTransform(transform_id);
        rtcontext->updateGPUTreenodesPath(rootIndex);
    }

    //std::cout << "Scene root node: " << rtcontext->recoverSceneRoot() << std::endl;
    //std::cout << "Nodes after a movement: " << std::endl;
    //for (int i = 0; i < rtcontext->getNodes()->size(); i++) {
    //    rtcontext->getNodes()->at(i).print(rtcontext->getNodes());
    //    rtcontext->getNodes()->at(i).printBB();
    //}

}

Matrix4x4* Mesh::getTransform() {
    return &rtcontext->getTransforms()->at(transform_id);
}
