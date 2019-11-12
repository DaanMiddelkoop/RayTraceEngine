#include "treevisualiser.h"

#include <iostream>
#include <fstream>

using namespace RT;

TreeVisualiser::TreeVisualiser(std::string file_name, std::vector<Tree>* nodes)
{
    ofstream myfile;
    myfile.open (file_name);

    myfile << "digraph children {\n";
    for (int i = 0; i < nodes->size() && i < 10000; i++) {
        if (!nodes->at(i).leaf) {
            myfile << "\t" << i << " -> " << nodes->at(i).node1 << "\n";
            myfile << "\t" << i << " -> " << nodes->at(i).node2 << "\n";
        }
    }
    myfile << "}\n";

    myfile << "digraph parents {\n";
    for (int i = 0; i < nodes->size(); i++) {
        if (nodes->at(i).parent != -1) {
            myfile << "\t" << i << " -> " << nodes->at(i).parent << "\n";
        }
    }

    myfile << "}\n";
    myfile.close();
}
