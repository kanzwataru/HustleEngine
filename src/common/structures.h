#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "common/platform.h"

typedef struct Point {
    uint16 x, y;
} Point;

typedef struct Rect {
    uint16 x, y;
    uint16 w, h;
} Rect;

#endif
