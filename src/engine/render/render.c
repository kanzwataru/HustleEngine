#include "internal.h"
#include "engine/render.h"
#include "engine/asset.h"
#include "platform/video.h"
//#include "platform/mem.h"
#include "common/platform.h"
#include "common/math.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <math.h>

struct RenderIntr {
    Rect     *dirty_rects;
    byte      data[1];
};

static const Rect EMPTY_RECT = {0,0,0,0};
static bool video_initialized = false;

/*
 * Use a memory slot as a framebuffer
*/
static buffer_t *make_framebuffer(void) {
    //mem_alloc_block(memslot);
    //return mem_slot_get(memslot);
    return malloc(320 * 200);
}

void palette_set(const buffer_t *palette)
{
    video_set_palette(palette);
}

void palette_fade(const buffer_t *start, const buffer_t *end, float percent)
{
    int i;

    for(i = 0; i < PALETTE_NUM_COLORS; i += 1) {
        video_set_color_at(i, LERP(start[i * 3 + 0], end[i * 3 + 0], percent),
                              LERP(start[i * 3 + 1], end[i * 3 + 1], percent),
                              LERP(start[i * 3 + 2], end[i * 3 + 2], percent));
    }
}

void palette_fade_to_color(const buffer_t *start, Color end, float percent)
{
    int i;

    for(i = 0; i < PALETTE_NUM_COLORS; i += 1) {
        video_set_color_at(i, LERP(start[i * 3 + 0], end.r, percent),
                              LERP(start[i * 3 + 1], end.g, percent),
                              LERP(start[i * 3 + 2], end.b, percent));
    }
}
void palette_fade_from_color(Color start, const buffer_t *end, float percent)
{
    int i;

    for(i = 0; i < PALETTE_NUM_COLORS; i += 1) {
        video_set_color_at(i, LERP(start.r, end[i * 3 + 0], percent),
                              LERP(start.g, end[i * 3 + 1], percent),
                              LERP(start.b, end[i * 3 + 2], percent));
    }
}

static bool clip_rect(Rect *clipped, Point *offset, const Rect orig)
{
    int o_xmax = orig.x + orig.w;
    int o_ymax = orig.y + orig.h;

    if(orig.x > SCREEN_WIDTH || orig.y > SCREEN_HEIGHT) /* completely offscreen */
        return false;

    /* Horizontal clip */
    if(orig.x < 0) { /* left */
        if(o_xmax < 0) return false; /* completely offscreen */

        offset->x = -orig.x;

        clipped->x = 0;
        clipped->w = orig.w - offset->x;
    }
    else if(o_xmax > SCREEN_WIDTH) {  /* right */
        offset->x = 0;

        clipped->x = orig.x;
        clipped->w = SCREEN_WIDTH - orig.x;
    }
    else {                  /* not clipped */
        offset->x = 0;

        clipped->x = orig.x;
        clipped->w = orig.w;
    }

    /* Vertical clip */
    if(orig.y < 0) {
        if(o_ymax < 0) return false; /* completely offscreen */

        offset->y = -orig.y;

        clipped->y = 0;
        clipped->h = orig.h - offset->y;
    }
    else if(o_ymax > SCREEN_HEIGHT) {
        offset->y = 0;

        clipped->y = orig.y;
        clipped->h = SCREEN_HEIGHT - orig.y;
    }
    else {
        offset->y = 0;

        clipped->y = orig.y;
        clipped->h = orig.h;
    }

    return true;
}

/*
static void blit(buffer_t *dest, const buffer_t *src, Rect rect)
{
    register int y = rect.h;
    register int offset = CALC_OFFSET(rect.x, rect.y);

    for(; y > 0; --y) {
        memcpy(dest + offset, src + offset, rect.w);

        offset += SCREEN_WIDTH;
    }
}
*/

static void blit_offset(buffer_t *dest, const buffer_t *src, Rect rect, int offset, int orig_w)
{
    register int y = rect.h;
    dest += CALC_OFFSET(rect.x, rect.y);
    src += offset;

    for(; y > 0; --y) {
        memcpy(dest, src, rect.w);

        dest += SCREEN_WIDTH;
        src += orig_w;
    }
}
/*
static void tile_to_screen(buffer_t *dest, const buffer_t *src, Rect rect)
{
    register int y = rect.h;

    dest += CALC_OFFSET(rect.x, rect.y);

    for(; y > 0; --y) {
        memcpy(dest, src, rect.w);

        dest += SCREEN_WIDTH;
        src += rect.w;
    }
}

static void screen_to_tile(buffer_t *dest, const buffer_t *src, Rect rect)
{
    register int y = rect.h;

    src += CALC_OFFSET(rect.x, rect.y);

    for(; y > 0; --y) {
        memcpy(dest, src, rect.w);

        dest += rect.w;
        src += SCREEN_WIDTH;
    }
}
*/
static void screen_to_screen(buffer_t *dest, const buffer_t *src, Rect rect)
{
    register int y = rect.h;

    src += CALC_OFFSET(rect.x, rect.y);
    dest += CALC_OFFSET(rect.x, rect.y);

    for(; y > 0; --y) {
        memcpy(dest, src, rect.w);
        dest += SCREEN_WIDTH;
        src  += SCREEN_WIDTH;
    }
}

static void blit_offset_masked(buffer_t *dest, const buffer_t *src, Rect rect, int offset, int orig_w)
{
    register int x, y;

    dest += CALC_OFFSET(rect.x, rect.y);
    src += offset;

    for(y = 0; y < rect.h; ++y) {
        for(x = 0; x < rect.w; ++x) {
            if(src[x] != TRANSPARENT)
                *(dest + x) = *(src + x);
        }

        dest += SCREEN_WIDTH;
        src += orig_w;
    }
}

void draw_rect(buffer_t *buf, Rect rect, byte colour)
{
    register int y = rect.h;
    buf += CALC_OFFSET(rect.x,rect.y);

    for(; y > 0; --y) {
        memset(buf, colour, rect.w);
        buf += SCREEN_WIDTH;
    }
}

void draw_rect_clipped(buffer_t *buf, Rect rect, byte colour)
{
    Point _;

    if(clip_rect(&rect, &_, rect))
        draw_rect(buf, rect, colour);
    else
        return;
}

void draw_dot(buffer_t *buf, Point p, byte colour)
{
    p.x = CLAMP(p.x, 0, SCREEN_WIDTH);
    p.y = CLAMP(p.y, 0, SCREEN_HEIGHT);

    buf[CALC_OFFSET(p.x, p.y)] = colour;
}

/* Bresenham routine copied from: http://www.brackeen.com/vga/source/bc31/lines.c.html */
void draw_line(buffer_t *buf, const Point *p1, const Point *p2, const byte colour)
{
    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py, offset, count;

    dx = CLAMP(p2->x, 0, SCREEN_WIDTH)  - CLAMP(p1->x, 0, SCREEN_WIDTH);
    dy = CLAMP(p2->y, 0, SCREEN_HEIGHT) - CLAMP(p1->y, 0, SCREEN_HEIGHT);
    dxabs = abs(dx);
    dyabs = abs(dy);
    sdx = SGN(dx);
    sdy = SGN(dy);
    x = dyabs >> 1;
    y = dxabs >> 1;
    px = CLAMP(p1->x, 0, SCREEN_WIDTH);
    py = CLAMP(p1->y, 0, SCREEN_HEIGHT);

    offset = CALC_OFFSET(px, py);
    buf[offset] = colour;
    count = 1;

    if(dxabs >= dyabs) {                /* more horizontal */
        for(i = 0; i < dxabs; ++i, ++count) {
            y += dyabs;
            if(y >= dxabs) {
                y  -= dxabs;
                py += sdy;
            }

            px += sdx;
            offset = CALC_OFFSET(px, py);

            /* clip to screen boundary */
            if(px <= 0 || px >= SCREEN_WIDTH || py <= 0 || py >= SCREEN_HEIGHT)
                continue;

            buf[offset] = colour;
        }
    }
    else {                              /* more vertical */
        for(i = 0; i < dyabs; ++i, ++count) {
            x += dxabs;
            if(x >= dyabs) {
                x  -= dyabs;
                px += sdx;
            }

            py += sdy;
            offset = CALC_OFFSET(px, py);

            /* clip to screen boundary */
            if(px <= 0 || px >= SCREEN_WIDTH || py <= 0 || py >= SCREEN_HEIGHT)
                continue;

            buf[offset] = colour;
        }
    }
}

Rect draw_sprite_explicit(buffer_t *buf, buffer_t * const image, Rect rect)
{
    Point offset;

    if(!clip_rect(&rect, &offset, rect))
        return EMPTY_RECT;

    blit_offset(buf, image, rect, offset.x + (offset.y * rect.w), rect.w);

    return rect;
}

void renderer_start_frame(RenderData *rd)
{
    Point _;
    int i;

    if(rd->flags & RENDER_BG_SOLID) {
        for(i = rd->sprite_count - 1; i >= 0; --i) {
            if(rd->sprites[i].flags & SPRITE_RLE)
                draw_rle_sprite_filled(rd->screen, rd->sprites[i].vis.rle, rd->dirty_rects[i], rd->bg.colour);
            else
                draw_rect(rd->screen, rd->dirty_rects[i], rd->bg.colour);
        }
    }
    else {
        for(i = rd->sprite_count - 1; i >= 0; --i) {
            if(rd->sprites[i].flags & SPRITE_RLE) {
                if(!clip_rect(&rd->dirty_rects[i], &_, rd->dirty_rects[i])) {
                    continue;
                }
            }

            screen_to_screen(rd->screen, rd->bg.image, rd->dirty_rects[i]);
        }
   }
}

static void anim_update(AnimInstance *anim, Sprite *sprite)
{
    int i;

    /* update counter and stuff */
    if(!(anim->frame_skip_counter --> 0)) {
        anim->frame += anim->playback_direction;

        if(anim->frame == anim->animation->count || anim->frame == UCHAR_MAX) /* Can get overflowed by ANIM_FLIPFLOP */
        {
            switch(anim->animation->flags << 1 >> 1)
            {
            case ANIM_LOOP:
                anim->frame = 0;
                anim->playback_direction = 1;
                break;
            case ANIM_DISAPPEAR:
                sprite->flags &= ~SPRITE_ACTIVE;
                anim->playback_direction = 0;
                break;
            case ANIM_ONCE:
                --anim->frame;
                anim->playback_direction = 0;
                break;
            case ANIM_FLIPFLOP:
                anim->playback_direction = -anim->playback_direction;
                break;
            }
        }

        anim->frame_skip_counter = anim->animation->frameskip;
    }

    /* update sprite visuals */
    switch(anim->animation->flags >> 1 & ~0xF0) {
    case SHEET_PIXELS:
        sprite->vis.image = (buffer_t *)anim->animation->data + ((anim->animation->width * anim->animation->height) * anim->frame);
        break;
    case SHEET_RLE:
        /* TODO: make this not stupidly inefficient */
        sprite->vis.rle = (RLEImage *)anim->animation->data;
        for(i = 0; i < anim->frame; ++i) {
            sprite->vis.rle = (RLEImage *)((char*)sprite->vis.rle + sprite->vis.rle->size + offsetof(struct RLEHeader, fgcol));
        }
        break;
    }
}

void renderer_refresh_sprites(RenderData *rd)
{
    Rect transformed;
    Point image_offset;
    size_t i;

    for(i = 0; i < rd->sprite_count; ++i) {
        /* skip sprites that aren't active */
        if(!(rd->sprites[i].flags & SPRITE_ACTIVE)) {
            rd->dirty_rects[i] = EMPTY_RECT;
            continue;
        }

        /* parent transform */
        transformed = rd->sprites[i].rect;
        if(rd->sprites[i].parent) {
            transformed.x += rd->sprites[i].parent->x;
            transformed.y += rd->sprites[i].parent->y;
        }

        /*
         * clip sprite, skip if offscreen
         * (RLE routines have their own clipping, so skip)
        */
        if(!(rd->sprites[i].flags & SPRITE_RLE)) {
            if(!clip_rect(&transformed, &image_offset, transformed)) {
                rd->dirty_rects[i] = EMPTY_RECT;
                continue;
            }
        }

        rd->dirty_rects[i] = transformed;

        /* animation */
        if(rd->sprites[i].anim.animation) {
            anim_update(&rd->sprites[i].anim, &rd->sprites[i]);
        }

        /* draw the sprite */
        switch((rd->sprites[i].flags << 4 >> 4) & ~SPRITE_ACTIVE) {
        case SPRITE_SOLID:
            draw_rect(rd->screen, transformed, rd->sprites[i].vis.colour);
            break;
        case SPRITE_MASKED:
            blit_offset_masked(rd->screen, rd->sprites[i].vis.image, transformed, image_offset.x + (image_offset.y * transformed.w), rd->sprites[i].rect.w);
            break;
        case SPRITE_RLE:
            draw_rle_sprite(rd->screen, rd->sprites[i].vis.rle, transformed);
            break;
        case SPRITE_MONORLE:
            draw_rle_sprite_mono(rd->screen, rd->sprites[i].vis.rle, transformed);
            break;
        default: /* just standard bitmap */
            blit_offset(rd->screen, rd->sprites[i].vis.image, transformed, image_offset.x + (image_offset.y * transformed.w), rd->sprites[i].rect.w);
            break;
        }
    }
}

void renderer_finish_frame(RenderData *rd)
{
    video_flip(rd->screen);
}

size_t renderer_tell_size(uint16 sprite_count, byte flags)
{
    size_t size = sizeof(RenderData) + (sizeof(Sprite) * sprite_count);

    if(flags & RENDER_PERSIST) {
        size += sizeof(Rect) * sprite_count;
    }

    return size;
}

RenderData *renderer_init(void *memory, uint16 sprite_count, byte flags, buffer_t *palette)
{
    RenderData *rd;
#ifdef DEBUG
    void *initial_mem = memory;
#endif
    size_t total_size = renderer_tell_size(sprite_count, flags);
    memset(memory, 0, total_size);

    rd = memory;
    memory = (char *)memory + sizeof(*rd);

    rd->screen = make_framebuffer();
    rd->sprite_count = sprite_count;
    rd->flags = flags;
    if(!(flags & RENDER_BG_SOLID))
        rd->bg.image = make_framebuffer();

    if(rd->screen) {
        if(!video_initialized) {
            video_init_mode(VIDEO_MODE_LOW256, 1);
            video_initialized = true;
        }

        if(sprite_count > 0) {
            rd->sprites = memory;
            memory = (char *)memory + sizeof(*rd->sprites) * sprite_count;

            if(flags & RENDER_PERSIST) {
                rd->dirty_rects = memory;
                memory = (char *)memory + sizeof(*rd->dirty_rects) * sprite_count;
            }
        }
        else {
            rd->sprites = NULL;
        }

        if(palette)
            video_set_palette(palette);
    }
    else {
        PANIC("out of mem\n");
    }

#ifdef DEBUG
    assert(((char *)memory - (char *)initial_mem) == total_size);
#endif

    return rd;
}

void renderer_quit(RenderData *rd, bool quit_video)
{
    if(quit_video)
        video_exit();
}
