#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include "raytracecontext.h"

namespace RT {
class RayTraceContext;

class Texture
{

    public:
        int width;
        int height;
        int start_of_texture;
        float padding1;


        Texture();
        void setData(int width, int height, uint8_t* data);
        void setPixel(int x, int y, int pixel);
        int getWidth();
        int getHeight();

    protected:

    private:
};

class TextureHandle
{
    RayTraceContext* rtcontext;

    public:
        TextureHandle(RayTraceContext* rtcontext, int texture_id);
        void setData(int width, int height, uint8_t* data);
        int getWidth();
        int getHeight();
        int texture_id;

};


}

#warning "TEST";

#endif // TEXTURE_H
