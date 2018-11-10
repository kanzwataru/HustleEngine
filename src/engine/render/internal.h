#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

/*
 * Calculates the offset into the buffer (y * SCREEN_WIDTH + x)
 * Using bit shifting instead of mults for a bit of optimization
 *
 * (assumes mode 13h 320x200)
*/
#define CALC_OFFSET(x, y)             (((y) << 8) + ((y) << 6) + (x))

/*
 * Macro that determines if a rect is off screen
 *
*/
#define OFFSCREEN(r) \
    ((r).y + (r).h <= 0 || (r).y >= SCREEN_HEIGHT || \
     (r).x + (r).w <= 0 || (r).x >= SCREEN_WIDTH)

#endif