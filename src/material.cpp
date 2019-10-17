#include "material.h"

using namespace RT;

Material::Material()
{
    //ctor
    texture_id = -1;
    color_tint_red = 1.0;
    color_tint_green = 1.0;
    color_tint_blue = 1.0;
}


MaterialHandle::MaterialHandle(RayTraceContext* rtcontext, int material_id) {
    this->rtcontext = rtcontext;
    this->material_id = material_id;
}

void MaterialHandle::setTexture(TextureHandle* th) {
    this->rtcontext->getMaterial(material_id)->texture_id = th->texture_id;
    this->rtcontext->updateGPUMaterials();
}
