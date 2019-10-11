#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <algorithm>
#include <vector>
#include "Triangle.h"

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

    float offsetx;
    float offsety;
    float offsetz;
    int leaf_id;

    float rotatex;
    float rotatey;
    float rotatez;
    int node1;

    int node2;
    float padding0;
    float padding1;
    float padding2;


    Tree();
    void insertNode(std::vector<Tree>* nodes, int parent, Tree node);
    void extendNode(Tree node);
    float areaMetric(Tree node);
    void setBoundaries(Triangle* t);
    float getArea();
};

#endif
