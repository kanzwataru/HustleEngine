#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "common/platform.h"

typedef struct Point {
    int16 x, y;
} Point;

typedef struct Rect {
    int16 x, y;
    int16 w, h;
} Rect;

#endif
