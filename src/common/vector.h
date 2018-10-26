#ifndef VECTOR_H
#define VECTOR_H

#include "common/prim.h"
#include <math.h>

static inline Point paddp(Point a, Point b) {
    Point v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    return v;
}

static inline Point psubp(Point a, Point b) {
    Point v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    return v;
}

static inline Point pmulp(Point a, Point b) {
    Point v;
    v.x = a.x * b.x;
    v.y = a.y * b.y;
    return v;
}

static inline Point pdivp(Point a, Point b) {
    Point v;
    v.x = a.x / b.x;
    v.y = a.y / b.y;
    return v;
}

static inline Point pshiftright(Point a, int shift) {
    Point v;
    v.x = a.x >> shift;
    v.y = a.y >> shift;
    return v;
}

static inline Point pshiftleft(Point a, int shift) {
    Point v;
    v.x = a.x << shift;
    v.y = a.y << shift;
    return v;
}

static inline Point paddi(Point a, int val) {
    Point v;
    v.x = a.x + val;
    v.y = a.y + val;
    return v;
}

static inline Point psubi(Point a, int val) {
    Point v;
    v.x = a.x - val;
    v.y = a.y - val;
    return v;
}

static inline Point pmuli(Point a, int val) {
    Point v;
    v.x = a.x * val;
    v.y = a.y * val;
    return v;
}

static inline Point pdivi(Point a, int val) {
    Point v;
    v.x = a.x / val;
    v.y = a.y / val;
    return v;
}

static inline Point pnormalize(Point p) {
    Point v;
    v.x = (p.x > 0) - (p.x < 0);
    v.y = (p.y > 0) - (p.y < 0);
    return v;
}

static inline int pmagnitude(Point p) {
    return (int)sqrt((p.x * p.x) +
                     (p.y * p.y));
}

static inline int pmagnitude_sqr(Point p) {
    return (int)((p.x * p.x) +
                 (p.y * p.y));
}

static inline int pdistance(Point a, Point b) {
    return (int)sqrt(((b.x - a.x) * (b.x - a.x)) +
                     ((b.y - a.y) * (b.y - a.y)));
}

#endif
