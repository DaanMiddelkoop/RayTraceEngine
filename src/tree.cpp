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
    transform_parent = -1;

    isObject = false;
}

void Tree::insertNode(std::vector<Tree>* nodes, int own_id, Tree node) {
    if (isObject) {
        std::cout << "founnd object leaf" << std::endl;
        printBB();

        Tree newRoot = (*this);
        newRoot.extendNode(node);

        newRoot.node1 = own_id;
        newRoot.node2 = nodes->size() + 1;
        newRoot.parent = parent;

        newRoot.leaf = false;
        newRoot.isObject = false;
        newRoot.transform_id = -1;

        // Fix reference from this nodes parent to newRoot;
         if (parent != -1) {
            if (nodes->at(parent).node1 == (this - nodes->data())) {
                nodes->at(parent).node1 = nodes->size();
            } else if (nodes->at(parent).node2 == (this - nodes->data())) {
                nodes->at(parent).node2 = nodes->size();
            } else {
                std::cout << "BIG ERROR" << std::endl;
            }

            newRoot.transform_parent = nodes->at(parent).transform_parent;
        } else {
            newRoot.transform_parent = -1;
        }


        std::cout << "Setting depths of node " << node1 << std::endl;

        nodes->at(node1).setDepths(nodes);

        std::cout << "Setting depths of node " << node2 << std::endl;
        nodes->at(node2).setDepths(nodes);

        std::cout << "finished fixing depths " << std::endl;



        node.parent = nodes->size();
        parent = nodes->size();

        nodes->push_back(newRoot);
        nodes->push_back(node);


        newRoot.setDepths(nodes);

        return;
    }


    if (leaf) {
        if (leaf_id == -1) {
            *this = node;
        } else {
            leaf = false;
            // This node is a leaf, turn it into a node instead.
            Tree child1 = Tree();
            child1.leaf = true;
            child1.leaf_id = leaf_id;
            child1.copyBoundaries(this);
            child1.parent = own_id;
            child1.depth = depth + 1;
            node.depth = depth + 1;

            leaf_id = 0;

            node.parent = own_id;

            nodes->push_back(node);
            node1 = nodes->size() - 1;

            nodes->push_back(child1);
            node2 = nodes->size() - 1;
        }

         extendNode(node);
    } else {
        extendNode(node);
        // Select which child the new node should be added to.

        float a1 = nodes->at(node1).areaMetric(&node);
        float a2 = nodes->at(node2).areaMetric(&node);


        if (a1 < a2) {
            (*nodes)[node1].insertNode(nodes, node1, node);
        } else {
            (*nodes)[node2].insertNode(nodes, node2, node);
        }
        balance(nodes);

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

void Tree::balance(std::vector<Tree>* nodes) {
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

// Also returns the tree root.
int Tree::updateParents(std::vector<Tree>* nodes) {
    if (parent != -1) {
        Tree* p = &nodes->at(parent);
        p->recalculateBoundingBox(nodes);
        return p->updateParents(nodes);
    }

    if (parent == -1) {
        return this - nodes->data();
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

        if (nodes->at(parent).parent != -1) {
            if (nodes->at(nodes->at(parent).parent).node1 == parent) {
                nodes->at(nodes->at(parent).parent).node1 = sibbling;
            } else {
                nodes->at(nodes->at(parent).parent).node2 = sibbling;
            }
        }


        nodes->at(sibbling).parent = nodes->at(parent).parent;

        //std::cout << "updating parents..." << std::endl;
        // Update old bounding boxes.
        int root = nodes->at(sibbling).updateParents(nodes);
        //std::cout << "root: " << root << std::endl;

        //std::cout << "Removed self" << std::endl;

        //std::cout << "reinserting node: " << this - nodes->data() << " with sibbling: " << sibbling << " and parent: " << parent << std::endl;
        // Reinsert into tree.
        nodes->at(root).insertExistingNode(this - nodes->data(), nodes, parent);
    }

}

void Tree::fixTransformChildren(std::vector<Tree>* nodes, int old_parent, int new_parent) {
    if (transform_parent == old_parent) {
        transform_parent = new_parent;
    }

    nodes->at(node1).fixTransformChildren(nodes, old_parent, new_parent);
    nodes->at(node2).fixTransformChildren(nodes, old_parent, new_parent);
}

void Tree::insertExistingNode(int node, std::vector<Tree>* nodes, int extraNode) {
    if (leaf || isObject) {
        Tree* newRoot = &nodes->at(extraNode);
        *newRoot = (*this);
        newRoot->extendNode(nodes->at(node));

        newRoot->node1 = this - nodes->data();
        newRoot->node2 = node;
        newRoot->parent = parent;

        newRoot->leaf = false;
        newRoot->isObject = false;
        newRoot->transform_id = -1;

        // Fix reference from this nodes parent to newRoot;
         if (parent != -1) {
            if (nodes->at(parent).node1 == (this - nodes->data())) {
                nodes->at(parent).node1 = extraNode;
            } else if (nodes->at(parent).node2 == (this - nodes->data())) {
                nodes->at(parent).node2 = extraNode;
            } else {
                std::cout << "BIG ERROR" << std::endl;
            }


            newRoot->transform_parent = nodes->at(parent).transform_parent;
        } else {
            newRoot->transform_parent = -1;
        }


        nodes->at(node).parent = extraNode;
        parent = extraNode;

        newRoot->setDepths(nodes);

        return;




    } else {
        extendNode(nodes->at(node));
        float a1 = nodes->at(node1).areaMetric(&nodes->at(node));
        float a2 = nodes->at(node2).areaMetric(&nodes->at(node));


        if (a1 < a2) {
            (*nodes)[node1].insertExistingNode(node, nodes, extraNode);
        } else {
            (*nodes)[node2].insertExistingNode(node, nodes, extraNode);
        }
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

void Tree::updateTransformParents(std::vector<Tree>* nodes) {

    if (!leaf) {
        if (transform_id != -1) {
            nodes->at(node1).transform_parent = this - nodes->data();
            nodes->at(node2).transform_parent = this - nodes->data();

            std::cout << "Node detected with transform id, which is: " << this - nodes->data() << std::endl;


        } else {
            nodes->at(node1).transform_parent = transform_parent;
            nodes->at(node2).transform_parent = transform_parent;
        }
        if (nodes->at(node1).transform_id == -1) {
            nodes->at(node1).updateTransformParents(nodes);
        }

        if (nodes->at(node2).transform_id == -1) {
            nodes->at(node2).updateTransformParents(nodes);
        }
    }
}

void Tree::print(std::vector<Tree>* nodes) {
    std::cout << "Node " << this - nodes->data() << " --------------------------" << std::endl;
    std::cout << "parent" << parent << " node1 " << node1 << " node2 " << node2 << std::endl;
    std::cout << "Depth of node: " << depth << std::endl;
}

void Tree::printBB() {
    std::cout << "Bounding box:" << std::endl;
    std::cout << minx << " " << maxx << std::endl << miny << " " << maxy << std::endl << minz << " " << maxz << std::endl;
}
