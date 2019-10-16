#include <iostream>

#include "tree.h"

Tree::Tree() {
    leaf = true;
    leaf_id = -1;
    node1 = -1;
    node2 = -1;
    parent = -1;

    minx = 0.0f;
    miny = 0.0f;
    minz = 0.0f;

    maxx = 0.0f;
    maxy = 0.0f;
    maxz = 0.0f;

    offsetx = 0.0f;
    offsety = 0.0f;
    offsetz = 0.0f;

    rotatex = 0.0f;
    rotatey = 0.0f;
    rotatez = 0.0f;
}

void Tree::insertNode(std::vector<Tree>* nodes, int own_id, Tree node) {

    if (leaf) {
        if (leaf_id == -1) {
            leaf_id = node.leaf_id;
        } else {
            leaf = false;
            // This node is a leaf, turn it into a node instead.
            Tree child1 = Tree();
            child1.leaf_id = leaf_id;
            child1.extendNode(*this);
            child1.parent = own_id;

            nodes->push_back(child1);
            node1 = nodes->size() - 1;

            node.parent = own_id;
            nodes->push_back(node);
            node2 = nodes->size() - 1;
        }

    extendNode(node);
    } else {
    extendNode(node);
        // Select which child the new node should be added to.

        float a1 = nodes->at(node1).areaMetric(node);
        float a2 = nodes->at(node2).areaMetric(node);

        if (a1 < a2) {
            (*nodes)[node1].insertNode(nodes, node1, node);
        } else {
            (*nodes)[node2].insertNode(nodes, node2, node);
        }
    }
}

void Tree::extendNode(Tree node) {
    minx = min(minx, node.minx);
    miny = min(miny, node.miny);
    minz = min(minz, node.minz);

    maxx = max(maxx, node.maxx);
    maxy = max(maxy, node.maxy);
    maxz = max(maxz, node.maxz);
}

float Tree::areaMetric(Tree node) {
    float lx = min(minx, node.minx);
    float ly = min(miny, node.miny);
    float lz = min(minz, node.minz);

    float hx = max(maxx, node.maxx);
    float hy = max(maxy, node.maxy);
    float hz = max(maxz, node.maxz);

    return (max(hx - lx, 0.0001f) * max(hy - ly, 0.0001f) * max(hz - lz, 0.0001f)) - (max(maxx - minx, 0.0001f) * max(maxy - miny, 0.0001f) * max(maxz - minz, 0.0001f));
}

void Tree::setBoundaries(Triangle* t) {
    minx = min(t->x1, min(t->x2, t->x3));
    miny = min(t->y1, min(t->y2, t->y3));
    minz = min(t->z1, min(t->z2, t->z3));

    maxx = max(t->x1, max(t->x2, t->x3));
    maxy = max(t->y1, max(t->y2, t->y3));
    maxz = max(t->z1, max(t->z2, t->z3));

    if (maxx - minx < 0.00001f) {
        maxx += 0.000001f;
    }

    if (maxy - miny < 0.00001f) {
        maxy += 0.000001f;
    }

    if (maxz - minz < 0.00001f) {
        maxz += 0.000001f;
    }
}

float Tree::getArea() {
    return max(maxx - minx, 0.0001f) * max(maxy - miny, 0.0001f) * max(maxz - minz, 0.0001f);
}
