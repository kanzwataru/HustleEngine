#ifndef PLATFORM_H
#define PLATFORM_H

/* utility macros */
#define STRINGIFY_A(X) #X
#define STRINGIFY(X) STRINGIFY_A(X)

/* platform specific definitions */
#ifdef HE_PLATFORM_SDL2
    #include <stdbool.h>
    #include <stdint.h>
    #include <stdio.h>

    typedef uint8_t     buffer_t;
    typedef uint8_t     byte;
    typedef uint16_t    uint16;
    typedef int16_t     int16;
    typedef uint32_t    uint32;
    typedef int32_t     int32;
#endif

/* common definitions */
typedef struct PlatformData PlatformData;

#endif
