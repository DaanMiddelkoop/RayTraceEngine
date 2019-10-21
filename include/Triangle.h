#ifndef TRIANGLE_H
#define TRIANGLE_H


class Triangle
{
    public:
        Triangle();

        float x1;
        float y1;
        float z1;
        int material;

        float x2;
        float y2;
        float z2;
        float u3;

        float x3;
        float y3;
        float z3;
        float v3;

        float u1;
        float v1;
        float u2;
        float v2;

        float vn1x;
        float vn1y;
        float vn1z;
        float w1;

        float vn2x;
        float vn2y;
        float vn2z;
        float w2;

        float vn3x;
        float vn3y;
        float vn3z;
        float w3;

        float minx();
        float miny();
        float minz();

        float maxx();
        float maxy();
        float maxz();

    protected:

    private:


};

#endif // TRIANGLE_H
