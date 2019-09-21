#ifndef RENDER_H
#define RENDER_H

#include "common/platform.h"
#include "common/structures.h"

typedef struct Texture Texture;
typedef struct Framebuffer Framebuffer;

void renderer_init(PlatformData *pd);
void renderer_reloaded(PlatformData *pd);
void renderer_quit(PlatformData *pd);

void renderer_clear(byte clear_col);
void renderer_flip(void);

void renderer_draw_rect(Framebuffer *buf, Rect xform, byte color);

#endif
