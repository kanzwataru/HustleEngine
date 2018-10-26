#ifndef RENDER_H
#define RENDER_H
/*
 * HustleEngine Render routines
 *
*/

#include "common/platform.h"
#include "common/prim.h"

#define SCREEN_WIDTH 320   /* Mode 13h uses 320 x 200 res */
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE 64000u /* Amount of pixels (320 x 200) */
#define MAX_SPRITE_SIZE 128
#define MAX_LINE_LENGTH 512
#define TRANSPARENT 0
#define DEFAULT_VGA_PALETTE 0
#define FILL_BUFFER(buf, col) _fmemset((buf), (col), (SCREEN_SIZE));

/*
 * Only using low nibble for engine
 * flags, rest is for game use
 * */
enum SPRITEFLAGS {
     SPRITE_REFRESH    = 0x01, /* (0001) If the sprite should refresh */
     SPRITE_CLIP       = 0x02, /* (0010) If should be screen-clipped to not wrap back */
     SPRITE_FILL       = 0x04, /* (0100) If the sprite should be a colour and not an iamge */
     SPRITE_MASKED     = 0x08, /* (1000) If colour id 0 should be treated as transparent */
};

/*
 * Renderer Configuration flags
 * 
*/
enum RENDERFLAG {
     RENDER_DOUBLE_BUFFER = 0x01 /* (0000 0001) */
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
    union {
        buffer_t    *image;   /* not freed, reference only */
        byte         colour;
    } vis;
    Rect         rect;
    Rect         hitbox;
    Rect        *parent;      /* NULL by default, reference only */
    byte         flags;
    AnimInstance anim;        /* NULL by default, reference only */
} Sprite;

typedef struct {
    Rect         screen_clipping;
    buffer_t    *screen;
    buffer_t    *bg_layer;
    Sprite      *sprites;
    uint16         sprite_count;
    byte         flags;
} RenderData;

/**** Renderer functions ****/

buffer_t *create_image(uint16 w, uint16 h);
void destroy_image(buffer_t **image);
LineUndoList create_line_undo_list();
void destroy_line_undo_list(LineUndoList *lul);

int init_renderer(RenderData *rd, int sprite_count, buffer_t *palette);
int init_line_render(byte line_count);
void quit_renderer(RenderData *rd);

void start_frame(RenderData *rd);
void refresh_sprites(RenderData *rd);
void finish_frame(RenderData *rd);

void reset_sprite(Sprite *sprite);

/**** Drawing functions ****/

/*
 * Draw a filled rectangle with a specific colour
*/
void draw_rect(buffer_t *buf, const Rect *rect, byte colour);
void draw_rect_clipped(buffer_t *buf, const Rect *rect, byte colour);
void draw_dot(buffer_t *buf, Point p, byte colour);
Rect draw_sprite_explicit(buffer_t *buf, buffer_t * const image, const Rect rect, const Rect global_clip);
void draw_line(buffer_t *buf, LineUndoList undo, const Point *p1, const Point *p2, const byte colour);
void erase_line(buffer_t *buf, LineUndoList undo);

#endif /* RENDER_H */
