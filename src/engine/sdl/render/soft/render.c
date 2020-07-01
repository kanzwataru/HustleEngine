#include "internal.h"
#include "engine/render_soft.c"
#include "platform/sdl/nativeplatform.h"

static struct PlatformData *platform;
static struct RenderData *rd;

void renderer_init(struct PlatformData *pd)
{
    platform = pd;
    rd = &platform->renderer;
}

void renderer_quit(struct PlatformData *pd)
{
}

void renderer_flip(void)
{
    for(int i = 0; i < 320 * 200; ++i) {
        rd->rgba_buf[i] = rd->palette[backbuf[i]];
    }
}

void renderer_set_palette(const buffer_t *pal, byte offset, byte count)
{
    for(int i = offset; i < count; ++i) {
        rd->palette[i] = ((uint32_t)pal[i * 3 + 0] << 24) |
                         ((uint32_t)pal[i * 3 + 1] << 16) |
                         ((uint32_t)pal[i * 3 + 2] << 8)  |
                         (0xFF);
    }
}

void renderer_get_palette(buffer_t *pal, byte offset, byte count)
{
}
