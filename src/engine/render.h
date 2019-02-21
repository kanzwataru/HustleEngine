#ifndef RENDER_H
#define RENDER_H
/*
 * HustleEngine Render routines
 *
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
#define FILL_BUFFER(buf, col) _fmemset((buf), (col), (SCREEN_SIZE));

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
};

/*
 * Animation playback type
*/
enum ANIMTYPE {
     ANIM_LOOP       = 0,  /* play animation on loop */
     ANIM_ONCE       = 1,  /* play animation once then stop on the last frame */
     ANIM_DISAPPEAR  = 2,  /* play animation once then hide the sprite */
     ANIM_FLIPFLOP   = 3   /* play from beginning to end then end to beginning */
};

typedef void far* LineUndoList;

union Visual {
    buffer_t     *image;
    RLEImage     *rle;
    byte          colour;
};

typedef struct {
    buffer_t    *frames;      /* sprite sheet, not owned */
    size_t       frame_size;
    byte         count;
    byte         skip;
    byte         playback_type;
} Animation;

typedef struct {
    Animation   *animation;
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
    buffer_t    *screen;
    union Visual bg;          /* pointers owned */
    byte         flags;
    Rect         screen_clipping;
    Sprite      *sprites;
    uint16       sprite_count;
} RenderData;

/**** Renderer functions ****/

buffer_t *create_image(uint16 w, uint16 h);
void destroy_image(buffer_t **image);
LineUndoList create_line_undo_list();
void destroy_line_undo_list(LineUndoList *lul);

RenderData *renderer_init(int sprite_count, byte flags, buffer_t *palette);
void renderer_quit(RenderData *rd, bool quit_video);
void renderer_start_frame(RenderData *rd);
void renderer_draw_bg(RenderData *rd);
void renderer_refresh_sprites(RenderData *rd);
void renderer_finish_frame(RenderData *rd);

buffer_t *create_palette(void);
void palette_set(const buffer_t *palette);
void palette_fade(const buffer_t *start, const buffer_t *end, float percent);
void palette_fade_to_color(const buffer_t *start, Color end, float percent);
void palette_fade_from_color(Color start, const buffer_t *end, float percent);

void sprite_reset(Sprite *sprite);

/**** Drawing functions ****/

/*
 * Draw a filled rectangle with a specific colour
*/
void draw_rect(buffer_t *buf, const Rect rect, byte colour);
void draw_rect_clipped(buffer_t *buf, const Rect rect, byte colour);
void draw_dot(buffer_t *buf, Point p, byte colour);
Rect draw_sprite_explicit(buffer_t *buf, buffer_t * const image, const Rect rect, const Rect global_clip);
void draw_line(buffer_t *buf, LineUndoList undo, const Point *p1, const Point *p2, const byte colour);
void erase_line(buffer_t *buf, LineUndoList undo);

#endif /* RENDER_H */
