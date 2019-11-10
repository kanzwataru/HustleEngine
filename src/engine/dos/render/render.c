#include "internal.h"
#include "engine/init.h"
#include <mem.h>
#include <stdlib.h>

#define INPUT_STATUS_0      0x03da
#define PALETTE_INDEX       0x03c8

static volatile buffer_t *vga_mem = (volatile buffer_t *)0xA0000;
static buffer_t backbuf[320 * 200];

void renderer_init(struct PlatformData *pd)
{
    _asm {
        mov ax, 0x0013
        int 0x10
    }
}

void renderer_quit(struct PlatformData *pd)
{
    _asm {
        mov ax, 0x0003
        int 0x10
    }
}

void renderer_clear(byte clear_col)
{
    /* wait for vsync first */
    while(inp(INPUT_STATUS_0) & 8) {}
    while(!(inp(INPUT_STATUS_0) & 8)) {}

    memset((void *)backbuf, clear_col, 320 * 200);
}

void renderer_flip(void)
{
    memcpy((void *)vga_mem, backbuf, 320 * 200);
}

void renderer_set_palette(const buffer_t *pal, byte offset, byte count)
{
    int i;

    outp(PALETTE_INDEX, 0);
    for(i = offset; i < count * 3; ++i)
        outp(PALETTE_INDEX + 1, pal[i] >> 2);
}

void renderer_get_palette(buffer_t *pal, byte offset, byte count)
{

}

void renderer_draw_rect(Rect xform, byte color)
{
    int x, y;

    for(y = xform.y; y < xform.y + xform.h; ++y) {
        for(x = xform.x; x < xform.x + xform.w; ++x) {
            if(x > 0 && x < 320 && y > 0 && y < 200)
                backbuf[y * 320 + x] = color;
        }
    }
}
