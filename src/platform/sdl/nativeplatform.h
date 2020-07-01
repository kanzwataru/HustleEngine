#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include <SDL2/SDL.h>
#include "common/platform.h"
#include "common/mathlib.h"
#include "engine/sdl/render/soft/internal.h"
#include "engine/sdl/engine/internal.h"

#define WIDTH   320
#define HEIGHT  200

#define WITH_OPENGL 0

struct PlatformData {
    SDL_Window   *window_handle;
    SDL_GLContext gl_context;

    Rect screen_size;
    Rect target_size;

    struct EngineData engine;
    struct RenderData renderer;

#if !WITH_OPENGL
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;
#endif
};

#endif
