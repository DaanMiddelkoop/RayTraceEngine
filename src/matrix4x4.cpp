#include <stdio.h>
#include <cstring>
#include <math.h>

#include "matrix4x4.h"

Matrix4x4::Matrix4x4()
{
    for (int i = 0; i < 4; i++) {
        data[i][i] = 1.0f;
    }
}

Matrix4x4::Matrix4x4(float input[16]) {
    setData(input);
}

void Matrix4x4::setData(float* input)
{
    std::memcpy(&data, input, 16);
}

Matrix4x4 Matrix4x4::multiplicate(Matrix4x4* other) {
    Matrix4x4 result;

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            result.data[x][y] = multiplicate_row_col(other, x, y);
        }
    }
    return result;
}

int Matrix4x4::multiplicate_row_col(Matrix4x4* other, int row, int column) {
    int result = 0;
    for (int i = 0; i < 4; i++) {
        result += data[i][row] * other->data[column][i];
    }
    return result;
}

void Matrix4x4::rotate(float x, float y, float z) {
    float xs[16] = {1, 0, 0, 0,
                    0, cos(x), -sin(x), 0,
                    0, sin(x), cos(x), 0,
                    0, 0, 0, 1};

    float ys[16] = {cos(y), 0, sin(y), 0,
                    0, 1, 0, 0,
                    -sin(y), 0, cos(y), 0,
                    0, 0, 0, 1};

    float zs[16] = {cos(z), -sin(z), 0, 0,
                    sin(z), cos(z), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1};


    Matrix4x4 x_axis = Matrix4x4(xs);

    Matrix4x4 y_axis = Matrix4x4(ys);

    Matrix4x4 z_axis = Matrix4x4(zs);

    Matrix4x4 xy = x_axis.multiplicate(&y_axis);
    Matrix4x4 total = xy.multiplicate(&z_axis);

    *this = this->multiplicate(&total);
}

void Matrix4x4::setRotation(float x, float y, float z) {
    float xs[16] = {1, 0, 0, 0,
                    0, cos(x), -sin(x), 0,
                    0, sin(x), cos(x), 0,
                    0, 0, 0, 1};

    float ys[16] = {cos(y), 0, sin(y), 0,
                    0, 1, 0, 0,
                    -sin(y), 0, cos(y), 0,
                    0, 0, 0, 1};

    float zs[16] = {cos(z), -sin(z), 0, 0,
                    sin(z), cos(z), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1};


    Matrix4x4 x_axis = Matrix4x4(xs);

    Matrix4x4 y_axis = Matrix4x4(ys);

    Matrix4x4 z_axis = Matrix4x4(zs);

    Matrix4x4 xy = x_axis.multiplicate(&y_axis);
    *this = xy.multiplicate(&z_axis);
}

void Matrix4x4::setPosition(float x, float y, float z) {
    float xs[16] = {1, 0, 0, x,
                    0, 1, 0, y,
                    0, 0, 1, z,
                    0, 0, 0, 1};

    *this = Matrix4x4(xs);
}

void Matrix4x4::move(float x, float y, float z) {
    float xs[16] = {1, 0, 0, x,
                    0, 1, 0, y,
                    0, 0, 1, z,
                    0, 0, 0, 1};

    Matrix4x4 moveMatrix = Matrix4x4(xs);

    *this = this->multiplicate(&moveMatrix);
}
