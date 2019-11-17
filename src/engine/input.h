#ifndef INPUT_H
#define INPUT_H

#if defined(HE_PLATFORM_SDL2)
    #include "sdl/input.h"
#elif defined(HE_PLATFORM_DOS)
    #include "dos/input.h"
#endif

extern const bool *keyboard_keys;

#endif
