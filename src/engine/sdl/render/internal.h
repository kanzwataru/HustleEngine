#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H
#include "engine/sdl/render/gl.h"

struct RenderData {
    uint16 flags;

    byte palette[256 * 3];
    textureid_t palette_tex;
   
    Model quad;
    struct Framebuffer target_buf;
    struct Framebuffer back_buf;
    shaderid_t flat_shader;
    shaderid_t post_shader;
};

#endif
