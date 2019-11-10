#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H
#include "engine/render.h"

struct Framebuffer {
    buffer_t *buf;
    Point     size;
};

#endif
