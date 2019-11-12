#ifndef MATERIAL_H
#define MATERIAL_H

#include "raytracecontext.h"
#include "texture.h"

namespace RT {
    class RayTraceContext;
    class TextureHandle;

    class Material
    {
    public:
        int texture_id;
        float color_tint_red;
        float color_tint_green;
        float color_tint_blue;

        Material();

        protected:

        private:
    };

    class MaterialHandle
    {
        RayTraceContext* rtcontext;

        public:
            MaterialHandle(RayTraceContext* rtcontext, int material_id);

            void setTexture(TextureHandle* th);

            int material_id;
    };
}

#endif // MATERIAL_H
