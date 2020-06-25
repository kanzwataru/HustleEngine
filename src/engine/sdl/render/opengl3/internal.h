#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H
#include "engine/sdl/render/gl.h"

#define CACHE_MAX 65535

struct CachedTexture {
    bool cached;
    textureid_t tex;
};

struct RenderData {
    bool     initialized;
    uint16_t flags;

    byte palette[256 * 3];
    textureid_t palette_tex;

    struct Model quad;
    struct Framebuffer target_buf;
    struct Framebuffer back_buf;
    shaderid_t flat_shader;
    shaderid_t post_shader;
    shaderid_t sprite_shader;
    shaderid_t text_shader;
    shaderid_t tilemap_shader;

    struct CachedTexture cached_textures[CACHE_MAX];
};

#endif
