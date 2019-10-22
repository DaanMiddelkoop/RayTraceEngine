#ifndef MATRIX4X4_H
#define MATRIX4X4_H


class Matrix4x4
{
    public:
        Matrix4x4();
        Matrix4x4(float* input);
        void setData(float input[16]);
        Matrix4x4 multiplicate(Matrix4x4* other);
        void multiplicate(float* result, float* other);

        void setRotation(float x, float y, float z);
        void rotate(float x, float y, float z);

        void setPosition(float x, float y, float z);
        void move(float x, float y, float z);

        void print();

        // data[culumn][row];
        float data[4][4];

    protected:

    private:
        float multiplicate_row_col(Matrix4x4* other, int row, int column);

};

#endif // MATRIX4X4_H
