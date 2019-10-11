#ifndef TRIANGLE_H
#define TRIANGLE_H


class Triangle
{
    public:
        Triangle();

        float x1;
        float y1;
        float z1;

        float empty1;

        float x2;
        float y2;
        float z2;

        float empty2;

        float x3;
        float y3;
        float z3;

        float empty3;

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
