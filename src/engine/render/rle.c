#include "internal.h"
#include "platform/mem.h"
#include "engine/render.h"
#include "engine/render/rle.h"
#include "common/math.h"
#include <math.h>

static buffer_t linebuf[MAX_SPRITE_SIZE];

size_t buffer_to_rle(RLEImage *rle, buffer_t *buf)
{
    NOT_IMPLEMENTED
    return 0;
}

size_t monochrome_buffer_to_rle(RLEImageMono *rle, buffer_t *buf, int width, int height, byte bgcol, byte fgcol)
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

    DEBUG_DO(printf("* MonoRLE done (%zu bytes)\n", count * sizeof(struct MonoRLEChunk)));

    return count * sizeof(struct MonoRLEChunk);
}

void draw_mono_masked_rle(buffer_t *dest, const RLEImageMono *rle, const Rect * const rect, const byte col)
{
    int pcount, leftskip, rightlen;
    byte lines, lineskip;
    
    if(OFFSCREEN(*rect)) return;
    
    dest += CALC_OFFSET(MAX(rect->x, 0), rect->y);
    lines = (rect->y + rect->h) > SCREEN_HEIGHT ? SCREEN_HEIGHT - rect->y : rect->h;
    lineskip = rect->y < 0 ? abs(rect->y) : 0;

    /* fast-forward RLE to skip the clipped lines */
    while(lineskip != 0) {
        pcount = 0;
        while(pcount < rect->w) {
            pcount += rle->bglen + rle->fglen;
            ++rle;
        }
        --lineskip;
        --lines;
        dest += SCREEN_WIDTH;
    }

#define UNCOMPRESS_RLE(destbuf) \
    pcount = 0;                                        \
    while(pcount < rect->w) {                          \
        pcount += rle->bglen; /* skip BG */            \
        _fmemset((destbuf) + pcount, col, rle->fglen); \
                                                       \
        pcount += rle->fglen;                          \
        ++rle;                                         \
    }

    /* fall back to a slower drawing method if we need to clip horizontally */
    if(rect->x > 0 && rect->x + rect->w < SCREEN_WIDTH) {
        /* draw RLE directly */
        while(lines != 0) {
            UNCOMPRESS_RLE(dest);
            
            dest += SCREEN_WIDTH;
            --lines;
        }
    }
    else {
        leftskip = rect->x < 0 ? abs(rect->x) : 0;
        rightlen = (rect->x + rect->w) > SCREEN_WIDTH ? SCREEN_WIDTH - rect->x : rect->w;
        
        /* uncompress RLE then blit */
        while(lines != 0) {
            _fmemcpy(linebuf, dest, rightlen); /* can't rely on BG skip transparency :( */
            UNCOMPRESS_RLE(linebuf);           /* drop our RLE in the buffer */
            _fmemcpy(dest, linebuf + leftskip, rightlen); /* blit the buffer */
            
            dest += SCREEN_WIDTH;
            --lines;
        }
    }
#undef UNCOMPRESS_RLE
}
