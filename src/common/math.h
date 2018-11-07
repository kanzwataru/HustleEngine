#ifndef MATH_H
#define MATH_H

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CLAMP(val, min, max) (((val) > (max)) ? (max) : ((val < min)) ? (min) : (val))
#define SGN(a) ((a) > 0) - ((a) < 0)
#define LERP(a, b, t) ((a) + ((t) * ((b) - (a))))

#define TO_EVEN(x) (((x) + 1) & ~1)
#define DIV_CEIL(x, y) ((x) % (y)) ? (x) / (y) + 1 : (x) / (y);

#endif
