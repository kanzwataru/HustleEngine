#ifndef SDL_SHIM_H
#define SDL_SHIM_H

#ifdef SDL2
    #include <SDL2/SDL.h>
    
    #define SDL_GetKeyState(x) SDL_GetKeyboardState(x)
#else
    #include <SDL/SDL.h>
#endif

#endif
