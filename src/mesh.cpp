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

int Mesh::createRootNode() {
    Tree node = Tree();
    node.leaf = true;
    node.leaf_id = -1;
    rtcontext->getNodes()->push_back(node);
    return rtcontext->getNodes()->size() - 1;
}

void Mesh::build() {

    std::vector<Tree>* nodes = rtcontext->getNodes();
    nodes->reserve(nodes->size() + tend - tbegin);

    if (rootIndex != -1) {
        // Should first destroy the original model.


        // Falling back to just setting it to an empty leaf.
        nodes->at(rootIndex).leaf = true;
        nodes->at(rootIndex).leaf_id = -1;

    }

    // There are no triangles to insert.
    if (tend - tbegin == 0) {
        std::cout << "Warning: Tried building an empty triangle" << std::endl;
        return;
    }

    if (rootIndex == -1) {
        // Request new root node for this object.
        rootIndex = createRootNode();
    }

    nodes->at(rootIndex).isObject = false;

    // giving the root node the first triangle.
    nodes->at(rootIndex).leaf_id = tbegin;
    nodes->at(rootIndex).setBoundaries(&rtcontext->getTriangles()->at(tbegin));


    for (int i = tbegin + 1; i < tend; i++) {
        if (i % 1000 == 0) {
            std::cout << "Insert node " << i << " out of " << (tend - tbegin) << "\n";
        }


        // Update triangle material.

        rtcontext->getTriangles()->at(i).material = material_id;

        Tree node = Tree();
        node.leaf = true;
        node.leaf_id = i;
        node.setBoundaries(&rtcontext->getTriangles()->at(i));
        nodes->push_back(node);
        nodes->push_back(Tree());

        nodes->at(rootIndex).insertNode(nodes, nodes->size() - 2, nodes->size() - 1);

        if (nodes->at(nodes->size() - 1).parent == -1) {
            rootIndex = nodes->size() - 1;
        }
    }

    nodes->at(rootIndex).isObject = true;
    nodes->push_back(Tree());

    int sceneRoot = rtcontext->recoverSceneRoot();

    nodes->at(sceneRoot).insertNode(nodes, rootIndex, nodes->size() - 1);
    nodes->at(rootIndex).setDepths(nodes);

    rtcontext->recoverSceneRoot();
    rtcontext->updateGPUTreenodes();
    rtcontext->updateGPUTriangles();
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
        int root_transform_id = rtcontext->getNodes()->at(rootIndex).transform_id;

        rtcontext->getNodes()->at(rootIndex).transform_id = transform_id;
        getTransform()->node_id = rootIndex;


        if (root_transform_id != transform_id) {
            rtcontext->updateGPUTreenodes();
        }

        rtcontext->getNodes()->at(rootIndex).updateTransformBoundingBox(rtcontext->getNodes(), getTransform());

        rtcontext->updateGPUTransform(transform_id);
        rtcontext->updateGPUTreenodes();

        int sn = rootIndex;

        while (rtcontext->getNodes()->at(sn).parent != -1) {
            sn = rtcontext->getNodes()->at(sn).parent;
        }
        rtcontext->setSceneRoot(sn);
        //rtcontext->updateGPUTreenodesPath(rootIndex);
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
