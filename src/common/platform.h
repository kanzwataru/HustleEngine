#ifndef PLATFORM_H
#define PLATFORM_H

/* utility macros */
#define STRINGIFY_A(X) #X
#define STRINGIFY(X) STRINGIFY_A(X)

/* platform specific definitions */
#ifdef HE_PLATFORM_SDL2
    #include <stdbool.h>
#endif

#endif
