#include "Triangle.h"

#include <algorithm>

using namespace std;

Triangle::Triangle()
{
    //ctor
}

float Triangle::minx() {
    return min(x1, min(x2, x3));
}

float Triangle::miny() {
    return min(y1, min(y2, y3));
}

float Triangle::minz() {
    return min(z1, min(z2, z3));
}

float Triangle::maxx() {
    return max(x1, max(x2, x3));
}

float Triangle::maxy() {
    return max(y1, max(y2, y3));
}

float Triangle::maxz() {
    return max(z1, max(z2, z3));
}
