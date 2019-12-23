#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "common/platform.h"

typedef struct Point {
    int16_t x, y;
} Point;

typedef struct Rect {
    int16_t x, y;
    int16_t w, h;
} Rect;

bool math_clip_rect(Rect rect, const Rect * const bounds, Point *out_offset, Rect *out_rect);

#define ABS(_v)             ((_v) > 0 ? (_v) : -(_v))
#define CLAMP(_v, _mn, _mx) (((_v) > (_mx)) ? (_mx) : (((_v) < (_mn)) ? (_mn) : (_v))) 
#define SGN(_a)             ((_a) > 0) - ((_a) < 0) 

#endif
