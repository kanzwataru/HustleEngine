#include "internal.h"
#include "engine/init.h"
#include <mem.h>
#include <stdlib.h>

#define INPUT_STATUS_0      0x03da
#define PALETTE_INDEX       0x03c8

static volatile buffer_t *vga_mem = (volatile buffer_t *)0xA0000;
static buffer_t backbuf[320 * 200];
static const Rect bounds = {0, 0, 320, 200};

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
    memset((void *)backbuf, clear_col, 320 * 200);
}

void renderer_flip(void)
{
    while(inp(INPUT_STATUS_0) & 8) {}
    while(!(inp(INPUT_STATUS_0) & 8)) {}

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
    register buffer_t *buf = backbuf + (xform.y * 320 + xform.x);

    while(xform.h --> 0) {
        memset(buf, color, xform.w);

        buf += 320;
    }
}

void renderer_draw_texture(const buffer_t *texture, Rect xform)
{
    register buffer_t *buf;
    Rect clipped;
    Point offset;
    int x;

    if(math_clip_rect(xform, &bounds, &offset, &clipped)) {
        buf = backbuf + (clipped.y * 320 + clipped.x);
        texture += (offset.y * xform.w + offset.x);

        /*
        while(clipped.h --> 0) {
            memcpy(buf, texture, clipped.w);

            buf += 320;
            texture = (char *)texture + xform.w;
        }
        */
        while(clipped.h --> 0) {
            for(x = 0; x < clipped.w; ++x) {
                if(texture[x] != 0) {
                    buf[x] = texture[x];
                }
            }

            buf += 320;
            texture += xform.w;
        }
    }
}
