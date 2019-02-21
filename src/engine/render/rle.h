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

/* Convert bitmap to RLE */
size_t buffer_to_rle(RLEImage *rle, buffer_t *buf, int width, int height);

#endif
