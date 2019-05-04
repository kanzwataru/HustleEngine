#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

typedef unsigned char bool;
#define true 1
#define false 0

/***                      ***
 ***    COMPILER CHECK    ***
 ***                      ***/
#ifdef __WATCOMC__
    #define PLATFORM_DOS32
    #include <mem.h>
    #include <conio.h>
    #include <malloc.h>
    #include <dos.h>

    /* 32-bit MS-DOS */
    typedef unsigned char  byte;
    typedef unsigned short uint16;
    typedef unsigned int   uint32;
    typedef short          int16;
    typedef int            int32;

    typedef void __interrupt __far (*interrupt_t)(void);
#endif

#ifdef __GNUC__
    #define PLATFORM_SDL
    #include <string.h>
    #include <stdint.h>

    typedef uint8_t  byte;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef int16_t  int16;
    typedef int32_t  int32;
#endif

/* in practice this typedef will probably not be changed */
typedef byte buffer_t;

#endif
