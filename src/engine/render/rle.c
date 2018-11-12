#include "internal.h"
#include "platform/mem.h"
#include "engine/render.h"
#include "engine/render/rle.h"

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
    int pcount, left, right;
    byte lines, lineskip;
    byte dbgcol = 3;
    
    if(OFFSCREEN(*rect)) return;
    
    dest += CALC_OFFSET(rect->x, rect->y);
    lines = (rect->y + rect->h) > SCREEN_HEIGHT ? SCREEN_HEIGHT - rect->y : rect->h;
    lineskip = rect->y < 0 ? abs(rect->y) : 0;
    
    right = (rect->x + rect->w) > SCREEN_WIDTH ? SCREEN_WIDTH - rect->x : rect->w;
    left = rect->x < 0 ? rect->x : 0; /* this is negative! */
    right += left; /* either does nothing, or subtracts */

    assert(right >= 0);

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

/*
        while(1) {
            pcount += rle->bglen + rle->fglen;
            if(pcount >= 0) {
                
            }
        }
*/

        while(pcount < 0) {
            pcount += rle->bglen;
            if(pcount > 1) {
                _fmemset(dest + pcount + 1, 7, rle->fglen);
                pcount += rle->fglen;
                ++rle;
                break;
            }
            
            pcount += rle->fglen;
            if(pcount > 1) {
                _fmemset(dest, 9, pcount);
                ++rle;
                break;
            }
            
            ++rle;
        }

/*
        while(pcount < 0) {
            pcount += rle->bglen + rle->fglen;
            
            if(pcount > 0) {
                if(pcount - rle->fglen > 0) { // BG caused overflow 
                    _fmemset(dest + pcount - rle->fglen, 7, rle->fglen);
                }
                else { // FG caused overflow
                    _fmemset(dest, 9, pcount);
                }
            }
            
            ++rle;
        }
*/
        
        DEBUG_DO(printf("[%04d] left: %04d pcount: %04d right: %04d\n", rect->h - lines, left, pcount, right));
        assert(pcount <= right);
        assert(pcount >= 0);
        while(pcount < right) {
            pcount += rle->bglen; /* skip BG */
            
            if(pcount > right) {
                ++rle;
                break;
            }
            
            if(pcount + rle->fglen > right) {
                if(!(right - pcount > 0)) printf("right (%d) - pcount (%d) = %d\n", right, pcount, right - pcount);
                _fmemset(dest + pcount, dbgcol++, right - pcount);
                ++rle;
                break;
            }
            else {
                _fmemset(dest + pcount, dbgcol++, rle->fglen);
                pcount += rle->fglen;
                ++rle;
            }
        }
        
        dest += SCREEN_WIDTH;
        --lines;
    }
}
