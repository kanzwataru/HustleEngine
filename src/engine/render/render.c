#include "internal.h"
#include "engine/render.h"
#include "platform/video.h"
#include "platform/mem.h"
#include "common/platform.h"
#include "common/math.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

struct Pixel {
    int  x;
    int  y;
    byte col;
};

struct LineUndo {
    uint16         count;
    struct Pixel segs[MAX_LINE_LENGTH];
};

static Rect far *dirty_rects;
static const Rect EMPTY_RECT = {0,0,0,0};
static bool video_initialized = false;

memid_t   tblock = 0;
void far *transientmem = NULL;

/*
 * Use a memory slot as a framebuffer
*/
static buffer_t *make_framebuffer(slotid_t memslot) {
    mem_alloc_block(memslot);
    return mem_slot_get(memslot);
}

buffer_t *create_image(uint16 w, uint16 h) {
    return mem_pool_alloc(w * h);
}

buffer_t *create_palette(void) {
    return mem_pool_alloc(PALETTE_SIZE);
}

void destroy_image(buffer_t **image) {
    mem_pool_free(*image);
    *image = NULL;
}

LineUndoList create_line_undo_list() {
    return mem_pool_alloc(sizeof(struct LineUndo));
}

void destroy_line_undo_list(LineUndoList *lul) {
    mem_pool_free(*lul);
    *lul = NULL;
}

static void init_all_sprites(Sprite **sprites, const uint16 count) 
{
    if(count > 0) {
        *sprites = transientmem;
        
        dirty_rects = (Rect far *)((char far *)transientmem + (sizeof(Sprite) * count)); /* dirty tiles go right after the sprites */
    }
    else {
        *sprites = NULL;
    }
}

static void free_all_sprites(Sprite **sprites, uint16 *count)
{
    if(*sprites && *count > 0) {
        *sprites = NULL;
        dirty_rects = NULL;
    }

    *count = 0;
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

static bool clip_rect(Rect *clipped, Point *offset, const Rect orig, const Rect const *clip)
{
    int o_xmax = orig.x + orig.w;
    int o_ymax = orig.y + orig.h;
    int c_xmax = clip->x + clip->w;
    int c_ymax = clip->y + clip->h;

    if(orig.x > c_xmax || orig.y > c_ymax) /* completely offscreen */
        return false;

    /* Horizontal clip */
    if(orig.x < clip->x) { /* left */
        if(o_xmax < clip->x) return false; /* completely offscreen */
        
        offset->x = (clip->x - orig.x);

        clipped->x = clip->x;
        clipped->w = orig.w - offset->x;
    }
    else if(o_xmax > c_xmax) {  /* right */
        offset->x = 0;

        clipped->x = orig.x;
        clipped->w = c_xmax - orig.x;
    }
    else {                  /* not clipped */
        offset->x = 0;

        clipped->x = orig.x;
        clipped->w = orig.w;
    }

    /* Vertical clip */
    if(orig.y < clip->y) {
        if(o_ymax < clip->y) return false; /* completely offscreen */

        offset->y = (clip->y - orig.y);

        clipped->y = clip->y;
        clipped->h = orig.h - offset->y;
    }
    else if(o_ymax > c_ymax) {
        offset->y = 0;

        clipped->y = orig.y;
        clipped->h = c_ymax - orig.y;
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
        _fmemcpy(dest + offset, src + offset, rect.w);

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
        _fmemcpy(dest, src, rect.w);

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
        _fmemcpy(dest, src, rect.w);

        dest += SCREEN_WIDTH;
        src += rect.w;
    }
}

static void screen_to_tile(buffer_t *dest, const buffer_t *src, Rect rect)
{
    register int y = rect.h;

    src += CALC_OFFSET(rect.x, rect.y);

    for(; y > 0; --y) {
        _fmemcpy(dest, src, rect.w);

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
        _fmemcpy(dest, src, rect.w);
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
        _fmemset(buf, colour, rect.w);
        buf += SCREEN_WIDTH;
    }
}

void draw_rect_clipped(buffer_t *buf, Rect rect, byte colour)
{
    Rect screen_clip = {0,0,SCREEN_WIDTH, SCREEN_HEIGHT};
    Point _;

    if(clip_rect(&rect, &_, rect, &screen_clip))
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
void draw_line(buffer_t *buf, LineUndoList undo, const Point *p1, const Point *p2, const byte colour)
{
    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py, offset, count;
    struct Pixel *undopix = (*(struct LineUndo *)undo).segs;

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
    undopix[0].x = px;
    undopix[0].y = py;
    undopix[0].col = buf[offset];
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
                
            undopix[count].x = px;
            undopix[count].y = py;
            undopix[count].col = buf[offset];
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
            
            undopix[count].x = px;
            undopix[count].y = py;
            undopix[count].col = buf[offset];
            buf[offset] = colour;
        }
    }

    (*(struct LineUndo *)undo).count = count;
}

Rect draw_sprite_explicit(buffer_t *buf, buffer_t * const image, Rect rect, const Rect global_clip)
{
    Point offset;

    if(!clip_rect(&rect, &offset, rect, &global_clip))
        return EMPTY_RECT;

    blit_offset(buf, image, rect, offset.x + (offset.y * rect.w), rect.w);

    return rect;
}

void erase_line(buffer_t *buf, LineUndoList undo)
{
    uint16 count = (*(struct LineUndo *)undo).count;
    struct Pixel *p = (*(struct LineUndo *)undo).segs;

    if(!count)
        return;

    do {
        buf[CALC_OFFSET(p->x, p->y)] = p->col;
        p++;
    } while(--count);
}

void reset_sprite(Sprite *sprite) {
    _fmemset(sprite, 0, sizeof(Sprite));
}

void renderer_draw_bg(RenderData *rd)
{
    if(rd->flags & RENDER_BG_SOLID) {
        FILL_BUFFER(rd->screen, rd->bg.colour);
    }
    else {
        _fmemcpy(rd->screen, rd->bg.image, SCREEN_SIZE);
    }
}

void renderer_start_frame(RenderData *rd)
{
    int i;

    if(rd->flags & RENDER_BG_SOLID) {
        for(i = rd->sprite_count - 1; i >= 0; --i) {
            draw_rect(rd->screen, dirty_rects[i], rd->bg.colour);
        }
    }
    else {
        for(i = rd->sprite_count - 1; i >= 0; --i) {
            screen_to_screen(rd->screen, rd->bg.image, dirty_rects[i]);
        }
   }
}

static void anim_update(AnimInstance *anim, byte *sprite_flags)
{
    if(!(anim->frame_skip_counter --> 0)) {
        anim->frame += anim->playback_direction;

        if(anim->frame == anim->animation->count || anim->frame == UCHAR_MAX) /* Can get overflowed by ANIM_FLIPFLOP */
        {
            switch(anim->animation->playback_type) 
            {
            case ANIM_LOOP:
                anim->frame = 0;
                anim->playback_direction = 1;
                break;
            case ANIM_DISAPPEAR:
                *sprite_flags &= ~SPRITE_ACTIVE;
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

        anim->frame_skip_counter = anim->animation->skip;
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
            dirty_rects[i] = EMPTY_RECT;
            continue;
        }

        /* parent transform */
        transformed = rd->sprites[i].rect;
        if(rd->sprites[i].parent) {
            transformed.x += rd->sprites[i].parent->x;
            transformed.y += rd->sprites[i].parent->y;
        }
        
        /* clip sprite, skip if offscreen */
        if(!clip_rect(&transformed, &image_offset, transformed, &rd->screen_clipping)) {
            dirty_rects[i] = EMPTY_RECT;
            continue;
        }
        
        dirty_rects[i] = transformed;
        
        /* animation */
        if(rd->sprites[i].anim.animation) {
            rd->sprites[i].vis.image = rd->sprites[i].anim.animation->frames + (rd->sprites[i].anim.animation->frame_size * rd->sprites[i].anim.frame);

            anim_update(&rd->sprites[i].anim, &rd->sprites[i].flags);
        }
        
        /* draw the sprite */
        switch(rd->sprites[i].flags) {
        case 0x05: /* SPRITE_ACTIVE & SPRITE_SOLID */
            draw_rect(rd->screen, transformed, rd->sprites[i].vis.colour);
        case 0x09: /* SPRITE_ACTIVE & SPRITE_MASKED */
            blit_offset_masked(rd->screen, rd->sprites[i].vis.image, transformed, image_offset.x + (image_offset.y * transformed.w), rd->sprites[i].rect.w);
        case SPRITE_ACTIVE: /* just standard bitmap */
            //blit_offset(rd->screen, rd->sprites[i].vis.image, transformed, image_offset.x + (image_offset.y * transformed.w), rd->sprites[i].rect.w);
            break;
        default:
            break;
        }
    }
}

void renderer_finish_frame(RenderData *rd)
{
    video_flip(rd->screen);
}

RenderData *renderer_init(int sprite_count, byte flags, buffer_t *palette)
{
    RenderData *rd = mem_pool_alloc(sizeof(RenderData));
    tblock = mem_alloc_block(MEMSLOT_RENDERER_TRANSIENT);
    transientmem = mem_slot_get(MEMSLOT_RENDERER_TRANSIENT);
    assert(rd && tblock && transientmem);
    
    _fmemset(transientmem, 0, 64000);
    
    rd->screen = make_framebuffer(MEMSLOT_RENDERER_BACKBUFFER);
    rd->sprite_count = sprite_count;
    rd->flags = flags;
    if(!(flags & RENDER_BG_SOLID))
        rd->bg.image = make_framebuffer(MEMSLOT_RENDERER_BG);

    if(rd->screen) {
        if(!video_initialized) {
            video_init_mode(VIDEO_MODE_LOW256, 1);
            video_initialized = true;
        }

        init_all_sprites(&rd->sprites, sprite_count);

        rd->screen_clipping.x = 0;
        rd->screen_clipping.y = 0;
        rd->screen_clipping.w = SCREEN_WIDTH;
        rd->screen_clipping.h = SCREEN_HEIGHT;

        if(palette)
            video_set_palette(palette);
    }
    else {
        PANIC("out of mem\n");
    }

    return rd;
}

void renderer_quit(RenderData *rd, bool quit_video)
{
    free_all_sprites(&rd->sprites, &rd->sprite_count);
    rd->screen = NULL;
    
    assert(tblock);
    mem_free_block(tblock);
    tblock = 0;
    transientmem = NULL;
    
    if(quit_video)
        video_exit();
}
