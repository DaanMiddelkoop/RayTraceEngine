#include <iostream>

#include "texture.h"

using namespace RT;

Texture::Texture()
{
}

void Texture::setData(int width, int height, uint8_t* data) {
    this->width = width;
    this->height = height;
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

TextureHandle::TextureHandle(RayTraceContext* rtcontext, int texture_id) {
    this->texture_id = texture_id;
    this->rtcontext = rtcontext;
}

void TextureHandle::setData(int width, int height, uint8_t* data) {
    Texture* tex = rtcontext->getTexture(texture_id);

    std::cout << "Texture object size: !!!!!    " << sizeof(Texture) << "\n";
    if ((tex->width * tex->height) > 0) {
        // Remove old texture from the scene.
        //rtcontext->get
    }

    tex->width = width;
    tex->height = height;

    tex->start_of_texture = rtcontext->getPixels()->size();

    for (int i = 0; i < width * height; i++) {
        unsigned int color = (data[i * 4] << 24) |
                                (data[i * 4 + 1] << 16) |
                                (data[i * 4 + 2] << 8) |
                                (data[i * 4 + 3]);
        rtcontext->getPixels()->push_back(color);

    }


    //rtcontext->getPixels()->insert(rtcontext->getPixels()->end(), width * height, *((unsigned int*)data));

    rtcontext->updateGPUTextures();
    rtcontext->updateGPUPixels();
    std::cout << "Updating gpu textures and pixels" << std::endl;
}

int TextureHandle::getWidth() {
    return rtcontext->getTexture(texture_id)->getWidth();
}

int TextureHandle::getHeight() {
    return rtcontext->getTexture(texture_id)->getHeight();
}
