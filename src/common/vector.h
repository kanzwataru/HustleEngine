/*
 * Largely untested vector math library
 *
 * This could all be hopelessly broken
*/
#ifndef VECTOR_H
#define VECTOR_H

#include "common/prim.h"
#include <math.h>

/* *******************************************************
 *                                                       *
 *               Integer Vector (Point) Math             *
 *                                                       */
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
    v.x = SGN(p.x);
    v.y = SGN(p.y);
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

/* *******************************************************
 *                                                       *
 *               Floating Point Vector math              *
 *                                                       */
static inline Vec2D vaddv(Vec2D a, Vec2D b) {
    Vec2D v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    return v;
}

static inline Vec2D vsubv(Vec2D a, Vec2D b) {
    Vec2D v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    return v;
}

static inline Vec2D vmulv(Vec2D a, Vec2D b) {
    Vec2D v;
    v.x = a.x * b.x;
    v.y = a.y * b.y;
    return v;
}

static inline Vec2D vdivv(Vec2D a, Vec2D b) {
    Vec2D v;
    v.x = a.x / b.x;
    v.y = a.y / b.y;
    return v;
}

static inline Vec2D vshiftright(Point a, int shift) {
    Vec2D v;
    v.x = a.x >> shift;
    v.y = a.y >> shift;
    return v;
}

static inline Vec2D vshiftleft(Point a, int shift) {
    Vec2D v;
    v.x = a.x << shift;
    v.y = a.y << shift;
    return v;
}

static inline Vec2D vaddi(Point a, int val) {
    Vec2D v;
    v.x = a.x + val;
    v.y = a.y + val;
    return v;
}

static inline Vec2D vsubi(Point a, int val) {
    Vec2D v;
    v.x = a.x - val;
    v.y = a.y - val;
    return v;
}

static inline Vec2D vmuli(Point a, int val) {
    Vec2D v;
    v.x = a.x * val;
    v.y = a.y * val;
    return v;
}

static inline Vec2D vdivi(Point a, int val) {
    Vec2D v;
    v.x = a.x / val;
    v.y = a.y / val;
    return v;
}

static inline Vec2D vnormalize(Vec2D v) {
    Vec2D v;
    v.x = SGN(v.x);
    v.y = SGN(v.y);
    return v;
}

static inline float vmagnitude(Vec2D v) {
    return (float)sqrt((v.x * v.x) +
                       (v.y * v.y));
}

static inline float vmagnitude_sqr(Vec2D v) {
    return (float)((v.x * v.x) +
                   (v.y * v.y));
}

static inline float vdistance(Vec2D a, Vec2D b) {
    return (float)sqrt(((b.x - a.x) * (b.x - a.x)) +
                       ((b.y - a.y) * (b.y - a.y)));
}
/* ******************************************************* */

#endif
