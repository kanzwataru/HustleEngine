#include "engine/core.h"
#include "engine/render.h"
#include "engine/asset.h"
#include "platform/filesys.h"

static RenderData *rd;
static RLEImage *cloud;
static RLEImage *clipper;
static RLEImage *balloon;
static Rect prev_cloud_rect, cloud_rect;
static Rect prev_clipper_rect, clipper_rect;
static Rect prev_balloon_rect, balloon_rect;

static void *transientmem = NULL;
static byte *asset_pak = NULL;

#define CLIPPER_SPRITE_D    48

#define WCLOUDS_SPRITE_W    300
#define CLOUD_SPRITE_W      128
#define CLOUD_SPRITE_H      48
#define SKY_COL             1
#define CLOUD_COL           3
#define BALLOON_W           32

static void load_stuff(void)
{
    /*
    buffer_t *clipper_raw, *cloud_raw, *balloon_raw;
    uint16 size;

    cloud_raw = load_bmp_image("RES/CLOUD.BMP");
    cloud = calloc(1, CLOUD_SPRITE_H * CLOUD_SPRITE_W * 8);
    buffer_to_rle(cloud, cloud_raw, CLOUD_SPRITE_W, CLOUD_SPRITE_H);
    free(cloud_raw);

    clipper_raw = load_bmp_image("RES/CLIPPER.BMP");
    clipper = calloc(1, 4024 * 10);
    buffer_to_rle(clipper, clipper_raw, CLIPPER_SPRITE_D, CLIPPER_SPRITE_D);
    free(clipper_raw);

    balloon_raw = load_bmp_image("RES/BALLOON.BMP");
    balloon = calloc(1, 4096 * 10);
    size = buffer_to_rle(balloon, balloon_raw, BALLOON_W, BALLOON_W);
    free(balloon_raw);

    printf("size: %d\n", size);
    */
    asset_pak = load_asset_pak("assets.dat");

    cloud = (RLEImage *)((Spritesheet *)(asset_pak + ASSETS_CLOUD_SPRITESHEETRLE))->data;
    clipper = (RLEImage *)((Spritesheet *)(asset_pak + ASSETS_CLIPPER_SPRITESHEETRLE))->data;
    balloon = (RLEImage *)((Spritesheet *)(asset_pak + ASSETS_BALLOON_SPRITESHEETRLE))->data;
}

static void update(void)
{
    prev_cloud_rect = cloud_rect;
    //prev_wclouds_rect = wclouds_rect;
    prev_balloon_rect = balloon_rect;

    cloud_rect.x -= 1;
    if(cloud_rect.x + cloud_rect.w <= 0)
        cloud_rect.x = SCREEN_WIDTH + cloud_rect.w;

    balloon_rect.x -= 1;
    if(balloon_rect.x + balloon_rect.w <= 0)
        balloon_rect.x = SCREEN_WIDTH + balloon_rect.w;
}

static void render(void)
{
    renderer_start_frame(rd);

    draw_rle_sprite_filled(rd->screen, cloud, prev_cloud_rect, SKY_COL);
    draw_rle_sprite_filled(rd->screen, cloud, cloud_rect, CLOUD_COL);

    draw_rle_sprite_filled(rd->screen, clipper, prev_clipper_rect, SKY_COL);
    draw_rle_sprite_filled(rd->screen, clipper, clipper_rect, 23);
/*
    draw_rect_clipped(rd->screen, prev_wclouds_rect, SKY_COL);
    draw_mono_masked_rle(rd->screen, wclouds, wclouds_rect, CLOUD_COL);
*/
    draw_rle_sprite_filled(rd->screen, balloon, prev_balloon_rect, SKY_COL);
    draw_rle_sprite(rd->screen, balloon, balloon_rect);
}

static void render_flip(void)
{
    renderer_finish_frame(rd);
}

static bool input(void)
{
    if(keyboard_os_quit_event() || keyboard_keys[KEY_ESC])
        return false;

    prev_clipper_rect = clipper_rect;

    if(keyboard_keys[KEY_LEFT])   clipper_rect.x -= 1;
    if(keyboard_keys[KEY_RIGHT])  clipper_rect.x += 1;
    if(keyboard_keys[KEY_DOWN])   clipper_rect.y += 1;
    if(keyboard_keys[KEY_UP])     clipper_rect.y -= 1;

    return true;
}

static void quit(void)
{
    free(transientmem);
    free(asset_pak);
    renderer_quit(rd, true);
    engine_quit();
}

int rletest_start(void)
{
    CoreData cd;
    buffer_t *pal;

    engine_init();
    load_stuff();

    rd = 0;
    cd.update_callback = &update;
    cd.render_callback = &render;
    cd.flip_callback = &render_flip;
    cd.input_handler = &input;
    cd.exit_handler = &quit;

    transientmem = malloc(64000);

    pal = asset_pak + ASSETS_VGAPAL_PALETTE;
    rd  = renderer_init(transientmem, 3, RENDER_BG_SOLID | RENDER_PERSIST, pal);

    FILL_BUFFER(rd->screen, SKY_COL);
    rd->bg.colour = SKY_COL;
    cloud_rect.x = SCREEN_WIDTH - CLOUD_SPRITE_W;
    cloud_rect.y = 0;
    cloud_rect.w = CLOUD_SPRITE_W;
    cloud_rect.h = CLOUD_SPRITE_H;
    prev_cloud_rect = cloud_rect;

    //clipper_rect.x = SCREEN_WIDTH - CLIPPER_SPRITE_D / 2;
    clipper_rect.x = 0;
    clipper_rect.y = 100;
    clipper_rect.w = CLIPPER_SPRITE_D;
    clipper_rect.h = CLIPPER_SPRITE_D;

/*
    wclouds_rect.x = 0;
    wclouds_rect.y = SCREEN_HEIGHT - CLOUD_SPRITE_H;
    wclouds_rect.w = WCLOUDS_SPRITE_W;
    wclouds_rect.h = CLOUD_SPRITE_H;
*/

    balloon_rect.x = 0;
    balloon_rect.y = 30;
    balloon_rect.w = 32;
    balloon_rect.h = BALLOON_W;
    balloon_rect.h = BALLOON_W;

    engine_gameloop(cd);

    return 0;
}
