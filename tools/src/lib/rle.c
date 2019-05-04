#include "aconv.h"
#include "engine/render/internal.h"
#include "common/math.h"
#include <math.h>
#include <limits.h>

size_t buffer_to_rle(RLEImage *rle, buffer_t *buf, int width, int height)
{
    uint16 offset = 0, line = 0;
    size_t count = 0;

    DEBUG_DO(printf("*RLE Conversion *\n  buf: %p\n  size: (%d %d)\n", buf, width, height));

    rle->data[0].col = buf[0];

    while(offset < width * height) {
        while(line < width) {
            DEBUG_DO(printf("offset: %u line: %u count: %zu\n", offset, line, count));
            while(buf[offset + line] == rle->data[count].col) {
                ++rle->data[count].length;
                ++line;

                if(line == UCHAR_MAX)
                    goto next_rle;

                if(line == width)
                    goto next_line;
            }

            goto next_rle;
        }

    next_line:
        offset += line;
        line = 0;
    next_rle:
        ++count;
        rle->data[count].col = buf[offset + line];
    }

    DEBUG_DO(printf("* RLE done (%zu bytes)\n", count * sizeof(struct RLEChunk)));

    rle->size = count * sizeof(struct RLEChunk);
    return rle->size;
}
