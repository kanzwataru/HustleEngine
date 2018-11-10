#include "engine/rle.h"
#include "platform/mem.h"

struct RLEChunk {
    byte col;
    uint16 length;
}; 

struct MonoRLEChunk {
    uint16 bglen;
    uint16 fglen;  
}; 

RLEImage *buffer_to_rle(buffer_t *buf)
{
    NOT_IMPLEMENTED
}

RLEImageMono *monochrome_buffer_to_rle(buffer_t *buf, int width, int height, byte fgcol, byte bgcol)
{
    uint16 offset = 0, line = 0;
    struct MonoRLEChunk far *p;
    struct MonoRLEChunk far *rle;

    rle = mem_pool_alloc((width * height) * 2, sizeof(struct MonoRLEChunk));
    p = rle;
    assert(rle);

    printf("RLE time");

    while(offset < width * height) {
        while(line < width) {
            assert(buf[offset + line] == bgcol && buf[offset + line] == fgcol); /* catch broken sprites */
            while(buf[offset + line] == bgcol) {
                ++p->bglen;
                ++line;

                if(line == width)
                    goto next;
            }
            while(buf[offset + line] == fgcol) {
                ++p->fglen;
                ++line;

                if(line == width)
                    goto next;
            }

            ++p;
        }

    next:
        offset += line;
        line = 0;
        ++p;
    }

    return rle;
}
