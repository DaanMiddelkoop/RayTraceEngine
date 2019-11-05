#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <algorithm>
#include <vector>
#include "Triangle.h"
#include "matrix4x4.h"

using namespace std;

class Tree {
public:
    float minx;
    float miny;
    float minz;
    int parent;

    float maxx;
    float maxy;
    float maxz;
    int leaf;

    int leaf_id;
    int node1;
    int node2;
    int depth;

    int isObject;
    int transform_id;
    int transform_parent;
    float padding1;


    Tree();
    int getOwnId(std::vector<Tree>* nodes);

    void insertNode(std::vector<Tree>* nodes, int newNode, int extraNode);
    void extendNode(Tree node);
    float areaMetric(Tree* node);
    void setBoundaries(Triangle* t);
    float getArea();
    void copyBoundaries(Tree* tree);
    float getSurface();
    void updateParents(std::vector<Tree>* nodes);
    void updateTransformBoundingBox(std::vector<Tree>* nodes, Matrix4x4* transform);
    void setDepths(std::vector<Tree>* nodes);

    void balance(std::vector<Tree>* nodes);
    void balanceNodes(std::vector<Tree>* nodes);
    float unionSurface(Tree* node);
    void recalculateBoundingBox(std::vector<Tree>* nodes);
    void printBB();
    void print(std::vector<Tree>* nodes);

    void reinsert(std::vector<Tree>* nodes);
};

#endif
