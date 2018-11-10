#ifndef RLE_H
#define RLE_H

#include "common/platform.h"

struct RLEChunk {
    byte col;
    uint16 length;
}; 

struct MonoRLEChunk {
    uint16 bglen;
    uint16 fglen;  
}; 

typedef struct RLEChunk     far RLEImage;
typedef struct MonoRLEChunk far RLEImageMono;

size_t buffer_to_rle(RLEImage *rle, buffer_t *buf);
size_t monochrome_buffer_to_rle(RLEImageMono *rle, buffer_t *buf, int width, int height, byte fgcol, byte bgcol);

#endif
