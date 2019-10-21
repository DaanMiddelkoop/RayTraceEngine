#include "objloader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace RT;

ObjLoader::ObjLoader()
{
    //ctor
}

void ObjLoader::loadObjFile(std::string file_name)
{
    triangles.clear();

    std::cout << "file name: " << file_name << "\n";
    std::ifstream InputFileStream;
    InputFileStream.open(file_name, std::ifstream::in);


    std::cout << InputFileStream.fail() << " FAIL STATUS\n";


    while (InputFileStream.good()) {

        //std::cout << "something\n";
        std::string line;

        std::getline(InputFileStream, line);


        std::istringstream lineStream;
        lineStream.str(line);

        std::string line_id;
        std::getline(lineStream, line_id, ' ');

        //std::cout << line_id << std::endl;


        // Vertices
        if (line_id.compare("v") == 0) {
            Vertex vertex = Vertex();
            lineStream >> vertex.x;
            lineStream.get();
            lineStream >> vertex.y;
            lineStream.get();
            lineStream >> vertex.z;

            v.push_back(vertex);
        }

        // Vertice textures
        if (line_id.compare("vt") == 0) {
            Vertex v = Vertex();
            lineStream >> v.x;
            lineStream.get();
            lineStream >> v.y;
            lineStream.get();
            lineStream >> v.z;

            vt.push_back(v);
        }

        if (line_id.compare("vn") == 0) {
            Vertex v = Vertex();
            lineStream >> v.x;
            lineStream.get();
            lineStream >> v.y;
            lineStream.get();
            lineStream >> v.z;

            vn.push_back(v);
        }

        if (line_id.compare("vp") == 0) {
            Vertex v = Vertex();
            lineStream >> v.x;
            lineStream.get();
            lineStream >> v.y;
            lineStream.get();
            lineStream >> v.z;

            vp.push_back(v);
        }

        if (line_id.compare("f") == 0) {

            int v[3];
            int vt[3] = {-1, -1, -1};
            int vn[3] = {-1, -1, -1};

            for (int i = 0; i < 3; i++) {
                lineStream >> v[i];

                // read texture id.
                if (lineStream.get() == '/') {
                    if (!lineStream.peek() == '/')
                        lineStream >> vt[i];

                    // read vertex normals.
                    if (lineStream.get() == '/') {
                        lineStream >> vn[i];
                        lineStream.get(); // absolve space
                    }
                }
            }

            for (int i = 0; i < 3; i++) {
                v[i] -= 1;
                vt[i] -= 1;
                vn[i] -= 1;
            }

            buildTriangle(v, vt, vn);

            /*faces.push_back(v[0]);
            faces.push_back(v[1]);
            faces.push_back(v[2]);

            normals.push_back(vn[0]);
            normals.push_back(vn[1]);
            normals.push_back(vn[2]);

            texturecoords.push_back(vt[0]);
            texturecoords.push_back(vt[1]);
            texturecoords.push_back(vt[2]);*/
        }
    }
    std::cout << "Done reading file!\n";

}

void ObjLoader::inflate(Mesh* mesh)
{
    mesh->setTriangles(&this->triangles);
    mesh->build();
}

void ObjLoader::buildTriangle(int v[3], int vt[3], int vn[3]) {
    Triangle t;

    t.x1 = this->v.at(v[0]).x;
    t.y1 = this->v.at(v[0]).y;
    t.z1 = this->v.at(v[0]).z;

    t.x2 = this->v.at(v[1]).x;
    t.y2 = this->v.at(v[1]).y;
    t.z2 = this->v.at(v[1]).z;

    t.x3 = this->v.at(v[2]).x;
    t.y3 = this->v.at(v[2]).y;
    t.z3 = this->v.at(v[2]).z;

    if (vt[0] >= 0 && vt[1] >= 0 && vt[2] >= 0) {

        t.u1 = this->vt.at(vt[0]).x;
        t.u2 = this->vt.at(vt[0]).y;
        t.u3 = this->vt.at(vt[0]).z;

        t.v1 = this->vt.at(vt[1]).x;
        t.v2 = this->vt.at(vt[1]).y;
        t.v3 = this->vt.at(vt[1]).z;

        t.w1 = this->vt.at(vt[2]).x;
        t.w2 = this->vt.at(vt[2]).y;
        t.w3 = this->vt.at(vt[2]).z;
    }

    if (vn[0] >= 0 && vn[1] >= 0 && vn[2] >= 0) {
        t.vn1x = this->vn.at(vn[0]).x;
        t.vn1y = this->vn.at(vn[0]).y;
        t.vn1z = this->vn.at(vn[0]).z;

        t.vn2x = this->vn.at(vn[1]).x;
        t.vn2y = this->vn.at(vn[1]).y;
        t.vn2z = this->vn.at(vn[1]).z;

        t.vn3x = this->vn.at(vn[2]).x;
        t.vn3y = this->vn.at(vn[2]).y;
        t.vn3z = this->vn.at(vn[2]).z;
    }

    triangles.push_back(t);
}


























