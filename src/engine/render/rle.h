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
    byte length;
    byte col;
};

typedef struct RLEChunk far RLEImage;

size_t buffer_to_rle(RLEImage *rle, buffer_t *buf, int width, int height);

/* TODO: move to render.h */
void draw_rle(buffer_t *dest, const RLEImage *rle, Rect rect);
void draw_rle_filled(buffer_t *dest, const RLEImage *rle, Rect rect, byte col);
void draw_rle_filled_reverse(buffer_t *dest, const RLEImage *rle, Rect rect, byte col);
/* */

#endif
