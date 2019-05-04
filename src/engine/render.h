#ifndef RENDER_H
#define RENDER_H
/*
 * HustleEngine Render routines
 *
 * Can either use renderer's builtin sprite drawing
 * or the individual drawing routines
*/

#include "common/platform.h"
#include "common/prim.h"
#include "render/rle.h"

#define SCREEN_WIDTH        320   /* Mode 13h uses 320 x 200 res */
#define SCREEN_HEIGHT       200
#define SCREEN_SIZE         64000u /* Amount of pixels (320 x 200) */
#define PALETTE_NUM_COLORS  256
#define PALETTE_SIZE        PALETTE_NUM_COLORS * 3
#define MAX_SPRITE_SIZE     128
#define MAX_LINE_LENGTH     512
#define TRANSPARENT         0
#define DEFAULT_VGA_PALETTE 0
#define FILL_BUFFER(buf, col) memset((buf), (col), (SCREEN_SIZE));
#define RESET_SPRITE(spr)     memset(spr, 0, sizeof(Sprite));

/*
 * Only using low nibble for engine
 * flags, rest is for game use
 * */
enum SPRITEFLAGS {
     SPRITE_ACTIVE     = 0x01, /* (0001) If the sprite should refresh */
     SPRITE_RLE        = 0x02, /* (0010) If this is an RLE sprite */
     SPRITE_MONORLE    = 0x06, /* (0110) If this is a solid RLE (monochrome) */
     SPRITE_SOLID      = 0x04, /* (0100) If this is a solid colour */
     SPRITE_MASKED     = 0x08, /* (1000) If colour id 0 should be treated as transparent */
};

/*
 * Renderer Flags
*/
enum RENDERFLAGS {
     RENDER_BG_SOLID  = 0x01, /* (0001) If set, the BG layer is a solid image */
     RENDER_PERSIST   = 0x02  /* (0010) If set, the screen will persist and dirty rects will be used */
};

/*
 * Forward-declarations
*/
typedef struct Spritesheet Spritesheet;
typedef struct RenderIntr RenderIntr;

union Visual {
    buffer_t    *image;
    RLEImage    *rle;
    byte         colour;
};

typedef struct {
    Spritesheet *animation;
    byte         frame;
    byte         frame_skip_counter;
    char         playback_direction;
} AnimInstance;

typedef struct {
    union Visual vis;         /* pointers not owned */
    byte         flags;
    Rect         rect;
    Rect         hitbox;
    Rect        *parent;      /* NULL by default, reference only */
    AnimInstance anim;        /* NULL by default, reference only */
} Sprite;

typedef struct {
    buffer_t    *screen;      /* back buffer, pointers owned */
    union Visual bg;           /* pointers owned */
    byte         flags;
    Sprite      *sprites;
    uint16       sprite_count;

    /* internal use only */
    Rect        *dirty_rects;
} RenderData;

/**** Renderer functions ****/
RenderData *renderer_init(void *memory, uint16 sprite_count, byte flags, buffer_t *palette);
size_t renderer_tell_size(uint16 sprite_count, byte flags);    /* how much space we need for these render settings */
void renderer_quit(RenderData *rd, bool quit_video);
void renderer_start_frame(RenderData *rd);
void renderer_refresh_sprites(RenderData *rd);
void renderer_finish_frame(RenderData *rd);

buffer_t *create_palette(void);
void palette_set(const buffer_t *palette);
void palette_fade(const buffer_t *start, const buffer_t *end, float percent);
void palette_fade_to_color(const buffer_t *start, Color end, float percent);
void palette_fade_from_color(Color start, const buffer_t *end, float percent);

/* *** DRAWING FUNCTIONS ***
 *
 * The renderer has sprite rendering logic but for
 * any effects that it cannot do, you may also use these drawing
 * functions manually
*/

/* Draw a filled rectangle with a specific colour */
void draw_rect(buffer_t *buf, const Rect rect, byte colour);
void draw_rect_clipped(buffer_t *buf, const Rect rect, byte colour);

/* Draw single pixels (with bounds-checking) */
void draw_dot(buffer_t *buf, Point p, byte colour);

/* Blit a bitmap (with bounds-checking) */
Rect draw_sprite_explicit(buffer_t *buf, buffer_t * const image, const Rect rect);

/* RLE (Run-Length Encoded) sprite drawing */
void draw_rle_sprite(buffer_t *dest, const RLEImage *image, Rect rect);
void draw_rle_sprite_mono(buffer_t *dest, const RLEImage *image, Rect rect);
void draw_rle_sprite_filled(buffer_t *dest, const RLEImage *image, Rect rect, byte col);
void draw_rle_sprite_filled_reverse(buffer_t *dest, const RLEImage *image, Rect rect, byte col);

/* Line drawing */
void draw_line(buffer_t *buf, const Point *p1, const Point *p2, const byte colour);

#endif /* RENDER_H */
