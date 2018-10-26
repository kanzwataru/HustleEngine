#ifndef PRIM_H
#define PRIM_H

typedef struct {
    int x, y;
} Point;

typedef struct {
    int x, y, w, h;
} Rect;

#define rect_collision(a, b) \
    ((a).x >= (b).x && (a).x <= (b).x + (b).w && \
     (a).y >= (b).y && (a).y <= (b).y + (b).h)    

#define hitbox_collision(a, ao, b, bo) \
    (((a).x + (ao).x) >= ((b).x + (bo).x) &&        \
     ((a).x + (ao).x) <= ((b).x + (bo).x) + (b).w && \
     ((a).y + (ao).y) >= ((b).y + (bo).y) &&          \
     ((a).y + (ao).y) <= ((b).y + (bo).y) + (b).h)     

static inline Rect calc_hitbox(Rect *hitbox, Rect *orig) {
    Rect r;
    r.x = hitbox->x + orig->x;
    r.y = hitbox->y + orig->y;
    r.w = hitbox->w;
    r.h = hitbox->h;
    return r;
}

#endif
