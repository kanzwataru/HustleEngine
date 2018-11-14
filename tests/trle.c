#include "engine/core.h"
#include "engine/render.h"
#include "platform/filesys.h"

static RenderData *rd;
static RLEImageMono *cloud;
static RLEImageMono *clipper;
static Rect prev_cloud_rect, cloud_rect;
static Rect prev_clipper_rect, clipper_rect;

#define CLIPPER_SPRITE_D    48

#define CLOUD_SPRITE_W      128
#define CLOUD_SPRITE_H      48
#define SKY_COL             1
#define CLOUD_COL           3

static void load_stuff(void)
{
    buffer_t *clipper_raw, *cloud_raw;
    cloud_raw = load_bmp_image("RES/CLOUD.BMP");
    cloud = mem_pool_alloc(CLOUD_SPRITE_H * CLOUD_SPRITE_W * 4);
    monochrome_buffer_to_rle(cloud, cloud_raw, CLOUD_SPRITE_W, CLOUD_SPRITE_H, SKY_COL, CLOUD_COL);
    destroy_image(&cloud_raw);
    
    clipper_raw = load_bmp_image("RES/CLIPPER.BMP");
    clipper = mem_pool_alloc(4024);
    monochrome_buffer_to_rle(clipper, clipper_raw, CLIPPER_SPRITE_D, CLIPPER_SPRITE_D, 0, 1);
    destroy_image(&clipper_raw);
}

static void monorle_dump(RLEImageMono *rle, int width, int height)
{
    int pcount = 0;
    int print_count = 0;
    
    printf("\nMonoRLE Dump\n");
    while(pcount < width * height) {
        printf("(%d %d)", rle->fglen, rle->bglen);
        if(++print_count == 4) {
            print_count = 0;
            printf("\n");
        }

        pcount += rle->fglen + rle->bglen;
        ++rle;
    }
    
    printf("\n");
}

static void update(void)
{
    prev_cloud_rect = cloud_rect;

    cloud_rect.x -= 1;
    if(cloud_rect.x + cloud_rect.w <= 0)
        cloud_rect.x = SCREEN_WIDTH + cloud_rect.w;
}

static void render(void)
{
    renderer_start_frame(rd);
    
    draw_rect_clipped(rd->screen, &prev_cloud_rect, SKY_COL);
    draw_mono_masked_rle(rd->screen, cloud, &cloud_rect, CLOUD_COL);
    
    draw_rect_clipped(rd->screen, &prev_clipper_rect, SKY_COL);
    draw_mono_masked_rle(rd->screen, clipper, &clipper_rect, 23);
}

static void render_flip(void)
{
    renderer_finish_frame(rd);
}

static bool input(void)
{
    keyboard_per_frame_update();
    if(keyboard_os_quit_event())
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
    mem_pool_free(cloud);
    engine_quit();
}

int rletest_start(void)
{
    CoreData cd;
    buffer_t *pal;
    
    engine_init();
    load_stuff();

#ifdef DEBUG
    monorle_dump(cloud, CLOUD_SPRITE_W, CLOUD_SPRITE_H);
#endif

    rd = 0;
    cd.update_callback = &update;
    cd.render_callback = &render;
    cd.flip_callback = &render_flip;
    cd.input_handler = &input;
    cd.exit_handler = &quit;
    
    pal = load_bmp_palette("RES/VGAPAL.BMP");
    rd  = renderer_init(3, RENDER_BG_SOLID, pal);
    destroy_image(&pal);
    
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
    
    engine_gameloop(cd);
    
    return 0;
}

