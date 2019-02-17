#ifndef RLE_H
#define RLE_H

#include "common/platform.h"

/*
 * Macro to get the size of the RLEImage.
 *
 * The first RLEChunk of every RLEImage is
 * actually a 16-bit integer with the size
 * (in bytes) of the entire image.
*/
#define GET_RLE_SIZE(rleptr) (*((uint16 *)(rleptr)))

struct RLEChunk {
    byte col;
    byte length;
};

struct MonoRLEChunk {
    byte bglen;
    byte fglen;
};

typedef struct RLEChunk     far RLEImage;
typedef struct MonoRLEChunk far RLEImageMono;

size_t buffer_to_rle(RLEImage *rle, buffer_t *buf, int width, int height);
size_t monochrome_buffer_to_rle(RLEImageMono *rle, buffer_t *buf, int width, int height, byte bgcol, byte fgcol);

/* TODO: move to render.h */
void draw_mono_masked_rle(buffer_t *dest, const RLEImageMono *rle, Rect rect, byte col);
void draw_mono_rle(buffer_t *dest, const RLEImageMono *rle, Rect rect, byte bgcol, byte fgcol);
void draw_masked_rle(buffer_t *dest, const RLEImage *rle, Rect rect);
/* */

#endif
