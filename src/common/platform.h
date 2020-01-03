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

    #define logdev      stdout
#endif

#ifdef HE_PLATFORM_DOS
    #include <stdio.h>
    #include <dos.h>
    #include <conio.h>
    #include <assert.h>

    typedef unsigned char   buffer_t;
    typedef unsigned char   byte;
    typedef unsigned short  uint16_t;
    typedef short           int16_t;
    typedef unsigned int    uint32_t;
    typedef int             int32_t;

    typedef int             bool;
    enum bool_vals_t { true = 1, false = 0 };

    typedef void __interrupt (*interrupt_t)(void);

    extern FILE *logdev;
#endif

#endif
