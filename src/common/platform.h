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
    #include <assert.h>

    typedef uint8_t     buffer_t;
    typedef uint8_t     byte;
    typedef uint16_t    uint16;
    typedef int16_t     int16;
    typedef uint32_t    uint32;
    typedef int32_t     int32;
#endif

#ifdef HE_PLATFORM_DOS
    #include <stdio.h>
    #include <dos.h>
    #include <conio.h>
    #include <assert.h>

    typedef unsigned char   buffer_t;
    typedef unsigned char   byte;
    typedef unsigned short  uint16;
    typedef short           int16;
    typedef unsigned int    uint32;
    typedef int             int32;

    typedef int             bool;
    enum bool_vals_t { true = 1, false = 0 };
#endif

/* common definitions */
typedef struct PlatformData PlatformData;

#endif
