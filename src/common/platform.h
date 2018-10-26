#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>

typedef unsigned char bool;
#define true 1
#define false 0

/***                      ***
 ***    COMPILER CHECK    ***
 ***                      ***/
#ifdef __WATCOMC__ /* watcom support */
    #include <mem.h>
    #include <conio.h>
    #define farmalloc(a)      _fmalloc(a)
    #define farcalloc(a, b)   _fcalloc(a, b)
    #define farfree(a)        _ffree(a)
    #define far __far
    #define MK_PTR(seg,off)   (((seg) << 4) + (off))
    #define inport(a)         inpw(a)
    #define inportb(a)        inp(a)
    #define outport(a, b)     outpw(a, b)
    #define outportb(a, b)    outp(a, b)
    #define getvect(a)        _dos_getvect(a)
    #define setvect(a, b)     _dos_setvect(a, b)
    //#define interrupt         __interrupt
#endif /* compiler */
/******************************************************/


/***                      ***
 ***    PLATFORM CHECK    ***
 ***                      ***/
#ifdef __GNUC__
    /* Modern 32-bit OS support (future) 
     *
     * DJGPP may also define this, so not guaranteed
     * to not be DOS, but DJGPP is not supported anyway
     */
    #define PLATFORM_SDL
    #include <string.h> /* memset is here on modern compilers */
    #include <stdint.h>
    #include <stdlib.h>
    
    #define far /* no more far pointers */
    #define farmalloc(a)        malloc(a)
    #define farcalloc(a, b)     calloc(a, b)
    #define farfree(a)          free(a)
    #define _fmemset(a, b, c)   memset(a, b, c)
    #define _fmemcpy(a, b, c)   memcpy(a, b, c)

    typedef uint8_t  buffer_t;
    typedef uint8_t  byte;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef int16_t  int16;
    typedef int32_t  int32;

    #define PANIC_DO(x)     do { x; exit(1); break; } while(0);
    #define PANIC(msg)      PANIC_DO(fprintf(stderr, "!! %s (%s %d)\n", (msg), __FILE__, __LINE__));
#else           
    /* 16-bit DOS
     *
     * This is for both Borland and Watcom
     */
    #define PLATFORM_DOS
    #include <malloc.h>
    #include <mem.h>
    #include <dos.h>
    #define inline /* no inline on C89 */

    typedef unsigned char far buffer_t;
    typedef unsigned char byte;
    typedef unsigned int  uint16;
    typedef unsigned long uint32;
    typedef int           int16;
    typedef long          int32;
    
    #define PANIC_DO(x)     while(1) { x; }
    #define PANIC(msg)      PANIC_DO(printf("!! %s (%s %d)\n", (msg), __FILE__, __LINE__));
#endif /* platform */
/******************************************************/
#ifdef DEBUG
    #define NOT_IMPLEMENTED \
        PANIC("Not implemented");    

    #define assert(expr) \
        if(expr) {} else PANIC_DO(printf("Assert fail %s, %d\n", __FILE__, __LINE__))
#else /* if not debug */
    #define NOT_IMPLEMENTED /* disabled */
    #define assert(expr)    /* disabled */
#endif

#endif /* PLATFORM_H */
