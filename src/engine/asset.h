/*
 * Include this file after the renderer header
 * to be able to use macros
*/
#ifndef ASSET_H
#define ASSET_H

#ifdef HUSTLE_RUNTIME
    #include "assets.gen.h"
#endif

/*
 * Spritesheet flags
*/
enum SHEETFLAGS {
     ANIM_LOOP       = 0x00,  /* play animation on loop */
     ANIM_ONCE       = 0x01,  /* play animation once then stop on the last frame */
     ANIM_DISAPPEAR  = 0x04,  /* play animation once then hide the sprite */
     ANIM_FLIPFLOP   = 0x08,  /* play from beginning to end then end to beginning */

     SHEET_RLE       = 0x10,  /* run-length encoded sprite */
     SHEET_PIXELS    = 0x40,  /* raw pixel data */
};

/*
 * Loadable sprite animation data
*/
typedef struct Spritesheet {
    uint16 width;
    uint16 height;
    byte   count;
    byte   frameskip;
    byte   flags;
    byte   data[1];         /* variable-size */
} Spritesheet;

/*
 * Loadable raw pixel image
*/
typedef struct {
    uint16 width;
    uint16 height;
    byte   flags;           /* currently unused, mostly for padding purposes */
    byte   data[1];         /* variable-size */
} Image;

/*
 * Loadable tilesheet data
*/
typedef struct {
    uint16 dimensions;
    byte   count;
    byte   data[1];         /* variable-size */
} Tileset;

/*
 * Loadable tile map data
*/
typedef struct {
    uint16 width;
    uint16 height;
    byte   tilesheet_id;
    byte   data[1];         /* variable-size */
} MapTable;

#endif /* ASSET_H */
