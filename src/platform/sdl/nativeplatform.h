#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include <SDL2/SDL.h>
#include "engine/sdl/render/gl.h"

#define WIDTH   320
#define HEIGHT  200

struct EngineData {
    uint16 flags;
    uint16 subsystems;
};

struct RenderData {
    uint16 flags;

    byte palette[256 * 3];
    textureid_t palette_tex;

    Model quad;
    struct Framebuffer indexed_buf;
    shaderid_t flat_shader;
    shaderid_t post_shader;
};

struct PlatformData {
    SDL_Window   *window_handle;
    SDL_GLContext gl_context;

    struct EngineData engine;
    struct RenderData renderer;
};

#endif
