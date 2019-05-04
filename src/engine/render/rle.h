/*
 * Run-Length Encoded sprite drawing
 *
 * Saves memory at the expense of CPU time, but can also
 * be faster to draw in certain circumstances because it uses
 * memory setting instead of memory copying
*/
#ifndef RLE_H
#define RLE_H

#include "common/platform.h"

struct RLEChunk {
    byte   length;
    byte   col;
};

struct RLEHeader {
    uint16 size;
    byte   bgcol;            /* set by user (for monochrome) */
    byte   fgcol;            /* set by user (for monochrome) */
    struct RLEChunk data[1]; /* variable length */
};

typedef struct RLEHeader RLEImage;

/* Convert bitmap to RLE */
size_t buffer_to_rle(RLEImage *rle, buffer_t *buf, int width, int height);

#endif
