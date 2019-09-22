#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include <SDL2/SDL.h>

struct EngineData {
    uint16 flags;
    uint16 subsystems;
};

struct RenderData {
    uint16 flags;
};

struct PlatformData {
    SDL_Window   *window_handle;
    SDL_GLContext gl_context;

    struct EngineData engine;
    struct RenderData renderer;
};

#endif
