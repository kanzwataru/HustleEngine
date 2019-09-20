#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include <SDL2/SDL.h>

struct PlatformData {
    SDL_Window   *window;
    SDL_Renderer *renderer;
};

#endif
