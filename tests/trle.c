#include "engine/core.h"
#include "engine/render.h"
#include "platform/filesys.h"

static RenderData *rd;
static RLEImageMono *cloud;
//static Rect prev_cloud_rect;
//static Rect cloud_rect;

#define CLOUD_SPRITE_W      128
#define CLOUD_SPRITE_H      48
#define SKY_COL             1
#define CLOUD_COL           3

static void load_stuff(void)
{
    buffer_t *cloud_raw = load_bmp_image("RES/CLOUD.BMP");
    cloud = mem_pool_alloc(CLOUD_SPRITE_H * CLOUD_SPRITE_W * 4);
    monochrome_buffer_to_rle(cloud, cloud_raw, CLOUD_SPRITE_W, CLOUD_SPRITE_H, SKY_COL, CLOUD_COL);
}
/*
static void update(void)
{
    
}

static void render(void)
{
}

static void render_flip(void)
{
}

static bool input(void)
{
    keyboard_per_frame_update();
    if(keyboard_os_quit_event())
        return false;
    
    return true;
}
*/
static void quit(void)
{
    mem_pool_free(cloud);
    engine_quit();
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

int rletest_start(void)
{
    //CoreData cd;
    
    engine_init();
    load_stuff();
    
    rd = 0;
    /*
    cd.update_callback = &update;
    cd.render_callback = &render;
    cd.flip_callback = &render_flip;
    cd.input_handler = &input;
    cd.exit_handler = &quit;
    */
    monorle_dump(cloud, CLOUD_SPRITE_W, CLOUD_SPRITE_H);
    quit();
    
    return 0;
}

