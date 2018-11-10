#include "platform/mem.h"
#include "engine/render.h"
#include "engine/render/rle.h"

size_t buffer_to_rle(RLEImage *rle, buffer_t *buf)
{
    NOT_IMPLEMENTED
    return 0;
}

size_t monochrome_buffer_to_rle(RLEImageMono *rle, buffer_t *buf, int width, int height, byte fgcol, byte bgcol)
{
    uint16 offset = 0, line = 0;
    size_t count = 0;

    DEBUG_DO(printf("* MonoRLE Conversion *\n  buf: %p\n  size: (%d %d)\n  fg: %d\n  bg:%d\n", buf, width, height, fgcol, bgcol));

    while(offset < width * height) {
        while(line < width) {
            DEBUG_DO(printf("offset: %u line: %u count: %zu\n", offset, line, count));
            //assert(buf[offset + line] == bgcol && buf[offset + line] == fgcol); /* catch broken sprites */
            while(buf[offset + line] == bgcol) {
                ++rle[count].bglen;
                ++line;

                if(line == width)
                    goto next;
            }
            while(buf[offset + line] == fgcol) {
                ++rle[count].fglen;
                ++line;

                if(line == width)
                    goto next;
            }

            ++count;
        }

    next:
        offset += line;
        line = 0;
        ++count;
    }

    DEBUG_DO(printf("* MonoRLE done (%zu bytes)\n", count));

    return count;
}
