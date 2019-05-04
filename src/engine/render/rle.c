#include "internal.h"
//#include "platform/mem.h"
#include "engine/render.h"
#include "engine/render/rle.h"
#include "common/math.h"
#include <math.h>
#include <limits.h>

/*
 * Massive macro that does the RLE drawing.
 * All the RLE drawing functions share 99% of the procedure so
 * a macro is the only way!!
*/
#ifdef DO_SECOND_TERM
    #define SECOND_TERM_A \
        else { memset(dest, BGCOL, pcount); }
    #define SECOND_TERM_B \
        else { memset(dest + pcount, BGCOL, MIN(rle->length, MAX(0, right - pcount))); }
    #define SECOND_TERM_C \
        else { memset(dest + pcount, COL, MIN(rle->length, MAX(0, right - pcount))); }
#else
    #define SECOND_TERM_A
    #define SECOND_TERM_B
    #define SECOND_TERM_C
#endif

#define DO_RLE_DRAW(COND, COL, BGCOL) \
    int pcount = 0, left, right;    \
    byte lines, lineskip;         \
    const struct RLEChunk *rle = &image->data[0]; \
                                    \
    if(OFFSCREEN(rect)) return;         \
                                                \
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
            if(COND) { memset(dest, COL, pcount); }       \
            SECOND_TERM_A                                   \
            ++rle;                                          \
            break;                                          \
        }                                                   \
                                                            \
        ++rle;                                              \
    }                                                       \
                                                            \
    if(left != 0) {                                         \
        while(pcount < right) { /* we've already fast-forwarded so draw to the end of the rle texture */    \
            if(COND) { memset(dest + pcount, COL, MIN(rle->length, MAX(0, right - pcount))); } \
            SECOND_TERM_B                                   \
            pcount += rle->length;                          \
            ++rle;                                          \
        }                                                   \
    }                                                       \
    else {                                                  \
        while(pcount < rect.w) { /* we started at the beginning so we have to fast-forward to the end once we hit the right clip */ \
            if(COND) { memset(dest + pcount, COL, MIN(rle->length, MAX(0, right - pcount))); } \
            SECOND_TERM_C                                   \
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


void draw_rle_sprite(buffer_t *dest, const RLEImage *image, Rect rect)
{
    DO_RLE_DRAW(rle->col != 0, rle->col, _);
}

void draw_rle_sprite_mono(buffer_t *dest, const RLEImage *image, Rect rect)
{
    #define DO_SECOND_TERM
    DO_RLE_DRAW(rle->col != 0, image->fgcol, image->bgcol);
    #undef DO_SECOND_TERM
}

void draw_rle_sprite_filled(buffer_t *dest, const RLEImage *image, Rect rect, byte col)
{
    DO_RLE_DRAW(rle->col != 0, col, _);
}

void draw_rle_sprite_filled_reverse(buffer_t *dest, const RLEImage *image, Rect rect, byte col)
{
    DO_RLE_DRAW(rle->col == 0, col, _);
}
