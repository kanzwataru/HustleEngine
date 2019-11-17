#include "mathlib.h"

/* TODO: note this is probably hopelessly broken if the bounds offset is non-zero */
bool math_clip_rect(Rect rect, const Rect * const bounds, Point *out_offset, Rect *out_rect)
{
    const int o_xmax = rect.x + rect.w;
    const int o_ymax = rect.y + rect.h;

    if(rect.x > bounds->w || rect.y > bounds->h) {
        /* early return, rectangle is completely off-screen */
        return false;
    }

    /* horizontal clip */
    if(rect.x < bounds->x) {
        /* left */
        if(o_xmax < bounds->x) return false; /* completely offscreen */

        out_offset->x = -rect.x;

        out_rect->x = 0;
        out_rect->w = rect.w - out_offset->x;
    }
    else if(o_xmax > bounds->w) {
        /* right */
        out_offset->x = 0;

        out_rect->x = rect.x;
        out_rect->w = bounds->w - rect.x;
    }
    else {
        /* not clipped */
        out_offset->x = 0;

        out_rect->x = rect.x;
        out_rect->w = rect.w;
    }

    /* vertical clip */
    if(rect.y < 0) {
        if(o_ymax < bounds->x) return false; /* completely offscreen */

        out_offset->y = -rect.y;

        out_rect->y = 0;
        out_rect->h = rect.h - out_offset->y;
    }
    else if(o_ymax > bounds->h) {
        out_offset->y = 0;

        out_rect->y = rect.y;
        out_rect->h = bounds->h - rect.y;
    }
    else {
        out_offset->y = 0;

        out_rect->y = rect.y;
        out_rect->h = rect.h;
    }

    return true;
}
