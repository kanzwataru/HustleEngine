#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

#include "common/platform.h"

struct RenderData {
    bool     initialized;
    uint16_t flags;

    uint32_t palette[256];
    uint32_t rgba_buf[320 * 200];
};

#endif

