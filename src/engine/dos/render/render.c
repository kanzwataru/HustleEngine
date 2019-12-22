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

void renderer_draw_rect(byte color, Rect xform)
{
    register buffer_t *buf = backbuf + (xform.y * 320 + xform.x);

    while(xform.h --> 0) {
        memset(buf, color, xform.w);

        buf += 320;
    }
}

static void blit_buffer(const buffer_t *texbuf, Rect xform)
{
    register buffer_t *buf;
    Rect clipped;
    Point offset;
    int x;

    if(math_clip_rect(xform, &bounds, &offset, &clipped)) {
        buf = backbuf + (clipped.y * 320 + clipped.x);
        texbuf += (offset.y * xform.w + offset.x);

        /*
        while(clipped.h --> 0) {
            memcpy(buf, texture, clipped.w);

            buf += 320;
            texture = (char *)texture + xform.w;
        }
        */
        while(clipped.h --> 0) {
            for(x = 0; x < clipped.w; ++x) {
                if(texbuf[x] != 0) {
                    buf[x] = texbuf[x];
                }
            }

            buf += 320;
            texbuf += xform.w;
        }
    }
}

void renderer_draw_texture(const struct TextureAsset *texture, Rect xform)
{
    // TODO: don't be blatantly slow
    blit_buffer(texture->data, xform);
}

#define ABS(_v)             ((_v) > 0 ? (_v) : -(_v))
#define CLAMP(_v, _mn, _mx) (((_v) > (_mx)) ? (_mx) : (((_v) < (_mn)) ? (_mn) : (_v))) 
#define SGN(_a)             ((_a) > 0) - ((_a) < 0) 
void renderer_draw_line(byte color, const Point *line, size_t count)
{
    /* brasenheim routine lifted from: http://www.brackeen.com/vga/source/bc31/lines.c.html */
    int ln, i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;
    for(ln = 1; ln < count; ++ln) {
        dx = CLAMP(line[ln].x, 0, 320) - CLAMP(line[ln - 1].x, 0, 320);
        dy = CLAMP(line[ln].y, 0, 200) - CLAMP(line[ln - 1].y, 0, 200);
        dxabs = ABS(dx);
        dyabs = ABS(dy);
        sdx = SGN(dx);
        sdy = SGN(dy);
        x = dyabs >> 1;
        y = dxabs >> 1;
        px = CLAMP(line[ln - 1].x, 0, 320);
        py = CLAMP(line[ln - 1].y, 0, 200);
        
        if(dxabs >= dyabs) {
            for(i = 0; i < dxabs; ++i) {
                y += dyabs;
                if(y >= dxabs) {
                    y  -= dxabs;
                    py += sdy;
                }

                px += sdx;

                if(px <= 0 || px >= 320 || py <= 0 || py >= 200)
                    continue;

               backbuf[320 * py + px] = color;
            }
        }
        else {
            for(i = 0; i < dyabs; ++i) {
                x += dxabs;
                if(x >= dyabs) {
                    x  -= dyabs;
                    px += sdx;
                }

                py += sdy;

                if(px <= 0 || px >= 320 || py <= 0 || py >= 200)
                    continue;

                backbuf[320 * py + px] = color;
            }
        }
    }
}

void renderer_draw_sprite(const struct SpritesheetAsset *sheet, const buffer_t *frame, Rect xform)
{
    blit_buffer(frame, xform);
}

void renderer_draw_tilemap(const struct TilemapAsset *map, const struct TilesetAsset *tiles, Point offset) 
{
    int ty, tx;
    Rect rect;
    const uint16_t *ids = (const uint16_t *)map->data;

    rect.w = tiles->tile_size;
    rect.h = tiles->tile_size;

    // TODO: do a better job of skipping off-screen tiles
    for(ty = 0; ty < map->height; ++ty) {
        for(tx = 0; tx < map->width; ++tx) {
            rect.x = (rect.w * tx) - offset.x;
            rect.y = (rect.h * ty) - offset.y;

            blit_buffer(&tiles->data[(rect.w * rect.h) * ids[ty * map->width + tx]], rect);
        }
    } 
}

