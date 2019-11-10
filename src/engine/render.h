#ifndef RENDER_H
#define RENDER_H

#include "common/platform.h"
#include "common/structures.h"

#define PALETTE_COLORS  256

struct Texture;
struct Framebuffer;

void renderer_clear(byte clear_col);
void renderer_flip(void);
void renderer_set_palette(const buffer_t *pal, byte offset, byte count);
void renderer_get_palette(buffer_t *pal, byte offset, byte count);
struct Framebuffer *renderer_get_backbuffer(void);

void renderer_draw_rect(struct Framebuffer *buf, Rect xform, byte color);

#endif
