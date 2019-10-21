#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <string>
#include "mesh.h"
#include "vertex.h"
#include "Triangle.h"

namespace RT {
    class ObjLoader
    {
        public:
            ObjLoader();
            void loadObjFile(std::string file_name);


            void inflate(Mesh* mesh);

        protected:

        private:
            std::vector<Vertex> v;
            std::vector<Vertex> vt;
            std::vector<Vertex> vn;
            std::vector<Vertex> vp;

            std::vector<int> faces;
            std::vector<int> normals;
            std::vector<int> texturecoords;

            std::vector<Triangle> triangles;

            void buildTriangle(int v[3], int vt[3], int vn[3]);


    };
}

#endif // OBJLOADER_H
