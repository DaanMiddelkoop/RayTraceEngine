#include <iostream>

#include "tree.h"

Tree::Tree() {
    leaf = true;
    leaf_id = -1;
    node1 = -1;
    node2 = -1;
    parent = -1;
    depth = 0;

    minx = 0.0f;
    miny = 0.0f;
    minz = 0.0f;

    maxx = 0.0f;
    maxy = 0.0f;
    maxz = 0.0f;

    transform_id = -1;

    isObject = false;
}

int Tree::getOwnId(std::vector<Tree>* nodes) {
    return this - nodes->data();
}

void Tree::insertNode(std::vector<Tree>* nodes, int leafNode, int extraNode) {

    if (isObject || leaf) {
        // prepare extraNode to be a new parent to both this node and the inserted node.
        nodes->at(extraNode).leaf = false;
        nodes->at(extraNode).isObject = false;
        nodes->at(extraNode).parent = parent;
        nodes->at(extraNode).depth = depth;
        nodes->at(extraNode).node1 = leafNode;
        nodes->at(extraNode).node2 = getOwnId(nodes);

        // create link from the original parent to the new "inbetween" node.
        if (parent != -1) {
            if (nodes->at(parent).node1 == getOwnId(nodes)) {
                nodes->at(parent).node1 = extraNode;
            } else {
                nodes->at(parent).node2 = extraNode;
            }
        }

        parent = extraNode;
        nodes->at(leafNode).parent = extraNode;

        nodes->at(extraNode).recalculateBoundingBox(nodes);
        nodes->at(extraNode).setDepths(nodes);

    } else {
        extendNode(nodes->at(leafNode));

        //Select which child the new node should be added to.

        float a1 = nodes->at(node1).areaMetric(&nodes->at(leafNode));
        float a2 = nodes->at(node2).areaMetric(&nodes->at(leafNode));


        if (a1 < a2) {
            nodes->at(node1).insertNode(nodes, leafNode, extraNode);
        } else {
            nodes->at(node2).insertNode(nodes, leafNode, extraNode);
        }
        balanceNodes(nodes);

    }
}

void Tree::setDepths(std::vector<Tree>* nodes) {
    if (parent != -1) {
        depth = nodes->at(parent).depth + 1;
    } else {
        depth = 0;
    }

    if (!leaf) {
        nodes->at(node1).setDepths(nodes);
        nodes->at(node2).setDepths(nodes);
    }
}

void Tree::balanceNodes(std::vector<Tree>* nodes) {
    // Four possible rotations:
    // http://box2d.org/files/GDC2019/ErinCatto_DynamicBVH_GDC2019.pdf rotations part


    // Rotate b and f;
    if (leaf || nodes->at(node1).isObject || nodes->at(node2).isObject) {
        return;
    }

    if (!nodes->at(node2).leaf) {

        float SA1 = nodes->at(node2).getSurface();
        float SA2 = nodes->at(node1).unionSurface(&(nodes->at(nodes->at(node2).node2)));

        // Rotation condition
        if (SA2 < SA1) {
            int new_node1 = nodes->at(node2).node1;
            nodes->at(node2).node1 = node1;
            node1 = new_node1;
            nodes->at(node2).recalculateBoundingBox(nodes);

            // fixing parents
            nodes->at(nodes->at(node2).node1).parent = node2;
            nodes->at(node1).parent = this - nodes->data();
        }
    }

    // Rotate c and e
    if (!nodes->at(node1).leaf) {

        float SA1 = nodes->at(node1).getSurface();
        float SA2 = nodes->at(node2).unionSurface(&(nodes->at(nodes->at(node1).node1)));

        // Rotation condition
        if (SA2 < SA1) {
            int new_node2 = nodes->at(node1).node2;
            nodes->at(node1).node2 = node2;
            node2 = new_node2;
            nodes->at(node1).recalculateBoundingBox(nodes);

            // fixing parents
            nodes->at(nodes->at(node1).node2).parent = node1;
            nodes->at(node2).parent = this - nodes->data();
        }
    }

    // Rotate b and g;
    if (!nodes->at(node2).leaf) {

        float SA1 = nodes->at(node2).getSurface();
        float SA2 = nodes->at(node1).unionSurface(&(nodes->at(nodes->at(node2).node1)));

        // Rotation condition
        if (SA2 < SA1) {
            int new_node1 = nodes->at(node2).node2;
            nodes->at(node2).node2 = node1;
            node1 = new_node1;
            nodes->at(node2).recalculateBoundingBox(nodes);

            nodes->at(nodes->at(node2).node2).parent = node2;
            nodes->at(node1).parent = this - nodes->data();
        }
    }

    // Rotate c and d
    if (!nodes->at(node1).leaf) {

        float SA1 = nodes->at(node1).getSurface();
        float SA2 = nodes->at(node2).unionSurface(&(nodes->at(nodes->at(node1).node2)));

        // Rotation condition
        if (SA2 < SA1) {
            int new_node2 = nodes->at(node1).node1;
            nodes->at(node1).node1 = node2;
            node2 = new_node2;
            nodes->at(node1).recalculateBoundingBox(nodes);

            nodes->at(nodes->at(node1).node1).parent = node1;
            nodes->at(node2).parent = this - nodes->data();
        }
    }
}

void Tree::balance(std::vector<Tree>* nodes) {
    balanceNodes(nodes);
    if (!leaf) {
        nodes->at(node1).balance(nodes);
        nodes->at(node2).balance(nodes);
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

float Tree::getSurface() {
    float width = (maxx - minx) * (maxy - miny);
    float length = (maxx - minx) * (maxz - minz);
    float height = (maxy - miny) * (maxz - minz);

    return 2.0f * ((width * height) + (width * length) + (length * height));
}

float Tree::unionSurface(Tree* node) {
    float lx = min(minx, node->minx);
    float ly = min(miny, node->miny);
    float lz = min(minz, node->minz);

    float hx = max(maxx, node->maxx);
    float hy = max(maxy, node->maxy);
    float hz = max(maxz, node->maxz);

    // Return surface area;
    float width = (hx - lx) * (hy - ly);
    float length = (hx - lx) * (hz - lz);
    float height = (hy - ly) * (hz - lz);

    return (2.0f * ((width * height) + (width * length) + (length * height)));
}

float Tree::areaMetric(Tree* node) {
    return unionSurface(node) - getSurface();

    //return max(hx - lx, 0.0001f) * max(hy - ly, 0.0001f) * max(hz - lz, 0.0001f);

    //return (max(hx - lx, 0.0001f) * max(hy - ly, 0.0001f) * max(hz - lz, 0.0001f)) - (max(maxx - minx, 0.0001f) * max(maxy - miny, 0.0001f) * max(maxz - minz, 0.0001f));
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

void Tree::recalculateBoundingBox(std::vector<Tree>* nodes) {


    minx = min(nodes->at(node1).minx, nodes->at(node2).minx);
    miny = min(nodes->at(node1).miny, nodes->at(node2).miny);
    minz = min(nodes->at(node1).minz, nodes->at(node2).minz);

    maxx = max(nodes->at(node1).maxx, nodes->at(node2).maxx);
    maxy = max(nodes->at(node1).maxy, nodes->at(node2).maxy);
    maxz = max(nodes->at(node1).maxz, nodes->at(node2).maxz);
}

void Tree::copyBoundaries(Tree* tree) {
    this->minx = tree->minx;
    this->miny = tree->miny;
    this->minz = tree->minz;

    this->maxx = tree->maxx;
    this->maxy = tree->maxy;
    this->maxz = tree->maxz;
}

float Tree::getArea() {
    return max(maxx - minx, 0.0001f) * max(maxy - miny, 0.0001f) * max(maxz - minz, 0.0001f);
}

void Tree::updateParents(std::vector<Tree>* nodes) {
    recalculateBoundingBox(nodes);

    if (parent != -1) {
        nodes->at(parent).recalculateBoundingBox(nodes);
    }
}


void Tree::reinsert(std::vector<Tree>* nodes) {
    // remove from the tree.
    //std::cout << "start with the reinsert " << std::endl;

    if (parent != -1) {


        if (nodes->at(parent).transform_id != -1) {
            std::cout << "HUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUGE PROBLEM" << std::endl;
            while (true) {};

        }

        int sibbling;
        if (nodes->at(parent).node1 == this - nodes->data()) {
            sibbling = nodes->at(parent).node2;
        } else {
            sibbling = nodes->at(parent).node1;
        }

        nodes->at(sibbling).parent = nodes->at(parent).parent;
        if (nodes->at(parent).parent != -1) {
            if (nodes->at(nodes->at(parent).parent).node1 == parent) {
                nodes->at(nodes->at(parent).parent).node1 = sibbling;
            } else {
                nodes->at(nodes->at(parent).parent).node2 = sibbling;
            }


            nodes->at(nodes->at(sibbling).parent).updateParents(nodes);
        }

        int root = sibbling;
        while (nodes->at(root).parent != -1) {
            root = nodes->at(root).parent;
        }
        nodes->at(sibbling).setDepths(nodes);

        nodes->at(root).insertNode(nodes, getOwnId(nodes), parent);
    }

}

void Tree::updateTransformBoundingBox(std::vector<Tree>* nodes, Matrix4x4* transform) {


    if (!leaf) {
        recalculateBoundingBox(nodes);
    }

    float p[8][4];

    p[0][0] = this->maxx;
    p[0][1] = this->maxy;
    p[0][2] = this->maxz;
    p[0][3] = 1;

    p[1][0] = this->minx;
    p[1][1] = this->maxy;
    p[1][2] = this->maxz;
    p[1][3] = 1;

    p[2][0] = this->maxx;
    p[2][1] = this->miny;
    p[2][2] = this->maxz;
    p[2][3] = 1;

    p[3][0] = this->maxx;
    p[3][1] = this->maxy;
    p[3][2] = this->minz;
    p[3][3] = 1;

    p[4][0] = this->minx;
    p[4][1] = this->miny;
    p[4][2] = this->maxz;
    p[4][3] = 1;

    p[5][0] = this->minx;
    p[5][1] = this->maxy;
    p[5][2] = this->minz;
    p[5][3] = 1;

    p[6][0] = this->maxx;
    p[6][1] = this->miny;
    p[6][2] = this->minz;
    p[6][3] = 1;

    p[7][0] = this->minx;
    p[7][1] = this->miny;
    p[7][2] = this->minz;
    p[7][3] = 1;


    float result[4];
    transform->multiplicate(result, p[0]);

    if (!leaf) {
        this->minx = result[0];
        this->maxx = result[0];

        this->miny = result[1];
        this->maxy = result[1];

        this->minz = result[2];
        this->maxz = result[2];


        for (int i = 1; i < 8; i++) {
            transform->multiplicate(result, p[i]);
            this->minx = min(this->minx, result[0]);
            this->miny = min(this->miny, result[1]);
            this->minz = min(this->minz, result[2]);

            this->maxx = max(this->maxx, result[0]);
            this->maxy = max(this->maxy, result[1]);
            this->maxz = max(this->maxz, result[2]);
        }
    } else {
        for (int i = 0; i < 8; i++) {
            transform->multiplicate(result, p[i]);
            this->minx = min(this->minx, result[0]);
            this->miny = min(this->miny, result[1]);
            this->minz = min(this->minz, result[2]);

            this->maxx = max(this->maxx, result[0]);
            this->maxy = max(this->maxy, result[1]);
            this->maxz = max(this->maxz, result[2]);
        }
    }
    reinsert(nodes);
}

void Tree::print(std::vector<Tree>* nodes) {
    std::cout << "Node " << this - nodes->data() << " --------------------------" << std::endl;
    std::cout << "parent" << parent << " node1 " << node1 << " node2 " << node2 << " leaf id " << leaf_id << std::endl;
    std::cout << "Depth of node: " << depth << std::endl;
}

void Tree::printBB() {
    std::cout << "Bounding box:" << std::endl;
    std::cout << minx << " " << maxx << std::endl << miny << " " << maxy << std::endl << minz << " " << maxz << std::endl;
}
