#ifndef RLE_H
#define RLE_H

#include "common/platform.h"

typedef struct RLEChunk     far RLEImage;
typedef struct MonoRLEChunk far RLEImageMono;

RLEImage *buffer_to_rle(buffer_t *buf);
RLEImageMono *monochrome_buffer_to_rle(buffer_t *buf, int width, int height, byte fgcol, byte bgcol);

#endif
