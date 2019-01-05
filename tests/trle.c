#include "engine/core.h"
#include "engine/render.h"
#include "platform/filesys.h"

static RenderData *rd;
static RLEImageMono *cloud;
static RLEImageMono *clipper;
static RLEImageMono *wclouds;
static Rect prev_cloud_rect, cloud_rect;
static Rect prev_clipper_rect, clipper_rect;
static Rect prev_wclouds_rect, wclouds_rect;

#define CLIPPER_SPRITE_D    48

#define WCLOUDS_SPRITE_W    300
#define CLOUD_SPRITE_W      128
#define CLOUD_SPRITE_H      48
#define SKY_COL             1
#define CLOUD_COL           3

static void convert_bw(buffer_t *buf, size_t size, byte bgcol, byte fgcol)
{
    size_t i;
    for(i = 0; i < size; ++i) {
        if(buf[i] != bgcol && buf[i] != fgcol)
            buf[i] = fgcol;
    }
}

static void load_stuff(void)
{
    buffer_t *clipper_raw, *cloud_raw, *wclouds_raw;
    uint16 size;
    
    cloud_raw = load_bmp_image("RES/CLOUD.BMP");
    cloud = mem_pool_alloc(CLOUD_SPRITE_H * CLOUD_SPRITE_W * 4);
    monochrome_buffer_to_rle(cloud, cloud_raw, CLOUD_SPRITE_W, CLOUD_SPRITE_H, SKY_COL, CLOUD_COL);
    destroy_image(&cloud_raw);
    
    clipper_raw = load_bmp_image("RES/CLIPPER.BMP");
    clipper = mem_pool_alloc(4024);
    monochrome_buffer_to_rle(clipper, clipper_raw, CLIPPER_SPRITE_D, CLIPPER_SPRITE_D, 0, 1);
    destroy_image(&clipper_raw);
    
    wclouds_raw = load_bmp_image("RES/CLOUDS.BMP");
    convert_bw(wclouds_raw, WCLOUDS_SPRITE_W * CLOUD_SPRITE_H, SKY_COL, CLOUD_COL);
    wclouds = mem_pool_alloc(4096 * 2);
    size = monochrome_buffer_to_rle(wclouds, wclouds_raw, WCLOUDS_SPRITE_W, CLOUD_SPRITE_H, SKY_COL, CLOUD_COL);
    destroy_image(&wclouds_raw);
    
    printf("size: %d\n", size);
    assert(size == GET_RLE_SIZE(wclouds));
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
    prev_wclouds_rect = wclouds_rect;

    cloud_rect.x -= 1;
    if(cloud_rect.x + cloud_rect.w <= 0)
        cloud_rect.x = SCREEN_WIDTH + cloud_rect.w;
    
    wclouds_rect.x -= 1;
    if(wclouds_rect.x + wclouds_rect.w <= 0)
        wclouds_rect.x = SCREEN_WIDTH + wclouds_rect.w;
}

static void render(void)
{
    renderer_start_frame(rd);
    
    draw_rect_clipped(rd->screen, prev_cloud_rect, SKY_COL);
    draw_mono_masked_rle(rd->screen, cloud, cloud_rect, CLOUD_COL);
    
    draw_rect_clipped(rd->screen, prev_clipper_rect, SKY_COL);
    draw_mono_masked_rle(rd->screen, clipper, clipper_rect, 23);
    
    draw_rect_clipped(rd->screen, prev_wclouds_rect, SKY_COL);
    draw_mono_masked_rle(rd->screen, wclouds, wclouds_rect, CLOUD_COL);
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
    monorle_dump(clipper, CLIPPER_SPRITE_D, CLIPPER_SPRITE_D);
    monorle_dump(wclouds, WCLOUDS_SPRITE_W, CLOUD_SPRITE_H);
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
    
    wclouds_rect.x = 0;
    wclouds_rect.y = SCREEN_HEIGHT - CLOUD_SPRITE_H;
    wclouds_rect.w = WCLOUDS_SPRITE_W;
    wclouds_rect.h = CLOUD_SPRITE_H;
    
    engine_gameloop(cd);
    
    return 0;
}

