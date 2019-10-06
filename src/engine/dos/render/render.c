#include "internal.h"
#include <mem.h>
#include <stdlib.h>

volatile buffer_t *vga_mem = (volatile buffer_t *)0xA0000;

void renderer_init(PlatformData *pd)
{
    _asm {
        mov ax, 0x0013
        int 0x10
    }
}

void renderer_quit(PlatformData *pd)
{
    _asm {
        mov ax, 0x0003
        int 0x10
    }
}

void renderer_reloaded(PlatformData *pd) {}

void renderer_clear(byte clear_col)
{
    memset((void *)vga_mem, 32, 320 * 200);
}

void renderer_flip(void)
{

}

void renderer_set_palette(const buffer_t *pal, byte offset, byte count)
{

}

void renderer_get_palette(buffer_t *pal, byte offset, byte count)
{

}

void renderer_draw_rect(Framebuffer *buf, Rect xform, byte color)
{

}

