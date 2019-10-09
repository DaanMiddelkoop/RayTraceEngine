#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <algorithm>
#include <vector>
#include "Triangle.h"

using namespace std;

class Tree {
public:

    Tree();


    bool leaf;
    int node1;
    int leaf_id;
    int node2;
    int parent;

    float minx;
    float miny;
    float minz;

    float maxx;
    float maxy;
    float maxz;

    float offsetx;
    float offsety;
    float offsetz;

    float rotatex;
    float rotatey;
    float rotatez;

    void insertNode(std::vector<Tree>* nodes, int parent, Tree node);
    void extendNode(Tree node);
    float areaMetric(Tree node);
    void setBoundaries(Triangle* t);
    float getArea();
};

#endif
