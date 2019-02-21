#include "internal.h"
#include "platform/mem.h"
#include "engine/render.h"
#include "engine/render/rle.h"
#include "common/math.h"
#include <math.h>
#include <limits.h>

#define SET_SIZE(rleptr, rlesize) (GET_RLE_SIZE(rleptr) = (rlesize))

/*
 * Massive macro that does the RLE drawing.
 * All the RLE drawing functions share 99% of the procedure so
 * a macro is the only way!!
*/
#define DO_RLE_DRAW(COND, COL) \
    int pcount, left, right;    \
    byte lines, lineskip;         \
                                    \
    if(OFFSCREEN(rect)) return;         \
                                                \
    ++rle; /* skip size header */                                   \
    dest += CALC_OFFSET(MAX(rect.x, 0), rect.y);                                \
    lines = (rect.y + rect.h) > SCREEN_HEIGHT ? SCREEN_HEIGHT - rect.y : rect.h;  \
    lineskip = rect.y < 0 ? abs(rect.y) : 0;                                       \
                                                                                    \
    right = (rect.x + rect.w) > SCREEN_WIDTH ? SCREEN_WIDTH - rect.x : rect.w;      \
    left = rect.x < 0 ? rect.x : 0; /* this is negative! */                         \
    right += left; /* either does nothing, or subtracts */                          \
                                                                                    \
    /* fast-forward RLE to skip the clipped lines */                                \
    while(lineskip != 0) {                                                         \
        pcount = 0;                                                               \
        while(pcount < rect.w) {                                                \
            pcount += rle->length;                                         \
            ++rle;                                                  \
        }                                                       \
        --lineskip;                                            \
        --lines;                                              \
        dest += SCREEN_WIDTH;                                \
    }                                                       \
    /* draw */                                              \
    while(lines != 0) {                                     \
    pcount = left; /* 0 if no clip, negative if clipping */ \
                                                            \
    while(pcount < 0) {                                     \
        pcount += rle->length;                              \
        if(pcount > 0) {                                    \
            if(COND)   /* skip transparent pixels */ \
              _fmemset(dest, COL, pcount);             \
            ++rle;                                          \
            break;                                          \
        }                                                   \
                                                            \
        ++rle;                                              \
    }                                                       \
                                                            \
    if(left != 0) {                                         \
        while(pcount < right) { /* we've already fast-forwarded so draw to the end of the rle texture */    \
            if(COND)   /* skip transparent pixels */   \
              _fmemset(dest + pcount, COL, MIN(rle->length, MAX(0, right - pcount))); \
            pcount += rle->length;                          \
            ++rle;                                          \
        }                                                   \
    }                                                       \
    else {                                                  \
        while(pcount < rect.w) { /* we started at the beginning so we have to fast-forward to the end once we hit the right clip */ \
            if(COND)   /* skip transparent pixels */   \
              _fmemset(dest + pcount, COL, MIN(rle->length, MAX(0, right - pcount)));  \
            pcount += rle->length;                          \
            ++rle;                                          \
        }                                                   \
    }                                                       \
                                                            \
    dest += SCREEN_WIDTH;                                   \
    --lines;                                                \
}

/* ***************** */
/* ***************** */
/* ***************** */
/* ***************** */
/* ***************** */
/* ***************** */


size_t buffer_to_rle(RLEImage *rle, buffer_t *buf, int width, int height)
{
    uint16 offset = 0, line = 0;
    size_t count = 1;

    DEBUG_DO(printf("*RLE Conversion *\n  buf: %p\n  size: (%d %d)\n", buf, width, height));

    rle[count].col = buf[0];

    while(offset < width * height) {
        while(line < width) {
            DEBUG_DO(printf("offset: %u line: %u count: %zu\n", offset, line, count));
            while(buf[offset + line] == rle[count].col) {
                ++rle[count].length;
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
        rle[count].col = buf[offset + line];
    }

    DEBUG_DO(printf("* RLE done (%zu bytes)\n", count * sizeof(struct RLEChunk)));

    SET_SIZE(rle, count * sizeof(struct RLEChunk));
    return GET_RLE_SIZE(rle);
}

void draw_rle(buffer_t *dest, const RLEImage *rle, Rect rect)
{
    DO_RLE_DRAW(rle->col != 0, rle->col);
}

void draw_rle_filled(buffer_t *dest, const RLEImage *rle, Rect rect, byte col)
{
    DO_RLE_DRAW(rle->col != 0, col);
}

void draw_rle_filled_reverse(buffer_t *dest, const RLEImage *rle, Rect rect, byte col)
{
    DO_RLE_DRAW(rle->col == 0, col);
}
