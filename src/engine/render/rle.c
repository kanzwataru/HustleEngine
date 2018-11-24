#include "internal.h"
#include "platform/mem.h"
#include "engine/render.h"
#include "engine/render/rle.h"
#include "common/math.h"
#include <math.h>
#include <limits.h>

#define SET_SIZE(rleptr, rlesize) (GET_RLE_SIZE(rleptr) = (rlesize))

size_t buffer_to_rle(RLEImage *rle, buffer_t *buf)
{
    NOT_IMPLEMENTED
    return 0;
}

size_t monochrome_buffer_to_rle(RLEImageMono *rle, buffer_t *buf, int width, int height, byte bgcol, byte fgcol)
{
    uint16 offset = 0, line = 0;
    size_t count = 1;

    DEBUG_DO(printf("* MonoRLE Conversion *\n  buf: %p\n  size: (%d %d)\n  fg: %d\n  bg:%d\n", buf, width, height, fgcol, bgcol));

    while(offset < width * height) {
        while(line < width) {
            DEBUG_DO(printf("offset: %u line: %u count: %zu\n", offset, line, count));
            //assert(buf[offset + line] == bgcol && buf[offset + line] == fgcol); /* catch broken sprites */
            while(buf[offset + line] == bgcol) {
                ++rle[count].bglen;
                ++line;

                if(line == UCHAR_MAX)
                    ++count;

                if(line == width)
                    goto next;
            }
            while(buf[offset + line] == fgcol) {
                ++rle[count].fglen;
                ++line;

                if(line == UCHAR_MAX)
                    ++count;

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
    
    SET_SIZE(rle, count * sizeof(struct MonoRLEChunk));
    return GET_RLE_SIZE(rle);
}

void draw_mono_masked_rle(buffer_t *dest, const RLEImageMono *rle, const Rect * const rect, const byte col)
{
    int pcount, left, right;
    byte lines, lineskip;
    
    if(OFFSCREEN(*rect)) return;
    
    ++rle; /* skip size header */
    dest += CALC_OFFSET(MAX(rect->x, 0), rect->y);
    lines = (rect->y + rect->h) > SCREEN_HEIGHT ? SCREEN_HEIGHT - rect->y : rect->h;
    lineskip = rect->y < 0 ? abs(rect->y) : 0;
    
    right = (rect->x + rect->w) > SCREEN_WIDTH ? SCREEN_WIDTH - rect->x : rect->w;
    left = rect->x < 0 ? rect->x : 0; /* this is negative! */
    right += left; /* either does nothing, or subtracts */

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

    /* draw */
    while(lines != 0) {
        pcount = left; /* 0 if no clip, negative if clipping */

        while(pcount < 0) {
            pcount += rle->bglen;
            if(pcount > 0) {
                _fmemset(dest + pcount, col, rle->fglen);
                pcount += rle->fglen;
                ++rle;
                break;
            }
            
            pcount += rle->fglen;
            if(pcount > 0) {
                _fmemset(dest, col, pcount);
                ++rle;
                break;
            }
            
            ++rle;
        }
        
        if(left != 0) {
            while(pcount < right) { /* we've already fast-forwarded so draw to the end of the rle texture */
                pcount += rle->bglen;
                
                _fmemset(dest + pcount, col, MIN(rle->fglen, MAX(0, right - pcount)));
                pcount += rle->fglen;
                ++rle;
            }
        }
        else {
            while(pcount < rect->w) { /* we started at the beginning so we have to fast-forward to the end once we hit the right clip */
                pcount += rle->bglen;
                
                _fmemset(dest + pcount, col, MIN(rle->fglen, MAX(0, right - pcount)));
                pcount += rle->fglen;
                ++rle;
            }
        }
        
        dest += SCREEN_WIDTH;
        --lines;
    }
}
