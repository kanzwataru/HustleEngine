#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include <SDL2/SDL.h>
#include "common/platform.h"
#include "common/mathlib.h"
#include "engine/sdl/render/soft/internal.h"
#include "engine/sdl/engine/internal.h"

#define WIDTH   320
#define HEIGHT  200

struct PlatformData {
    SDL_Window   *window_handle;
    SDL_GLContext gl_context;

    Rect screen_size;
    Rect target_size;
    
    struct EngineData engine;
    struct RenderData renderer;
};

#endif
