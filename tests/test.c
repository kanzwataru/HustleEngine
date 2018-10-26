#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "engine/render.h"
#include "engine/core.h"
#include "engine/event.h"
#include "platform/filesys.h"
#include "common/platform.h"
#include "platform/kb.h"

#define SPRITE_COUNT   8
#define BG_BRICK_SIZE 10
#define BG_BRICK_COL  4

static Point bounce_dirs[SPRITE_COUNT];
static RenderData rd;
//static byte col = 0;
static bool paused = false;

//static Animation test_anim;

static void engine_benchmark(CoreData cd, int times)
{
    clock_t start;
    double update_time;
    double render_time;
    double overall_time;
    int counter;

    start = clock();
    counter = times;
    while(--counter) {
        cd.update_callback();
    }
    update_time = (double)(clock() - start) / CLOCKS_PER_SEC;

    start = clock();
    counter = times;
    while(--counter) {
        cd.render_callback();
    }
    render_time = (double)(clock() - start) / CLOCKS_PER_SEC;


    start = clock();
    counter = times;
    while(--counter) {
        cd.update_callback();

        cd.render_callback();
    }
    overall_time = (double)(clock() - start) / CLOCKS_PER_SEC;

    quit_renderer(&rd);

    printf("engine benchmark: %d frames\n", times);
    printf("update %g seconds\n", update_time);
    printf("render %g seconds\n", render_time);
    printf("OVERALL: %g seconds\n", overall_time);

    exit(1);
}

void add_bricks(void)
{
    int col = 0;
    Rect r = {0,0,BG_BRICK_SIZE, BG_BRICK_SIZE};
    
    while(r.y < (SCREEN_HEIGHT - BG_BRICK_SIZE)) {
        while(r.x < (SCREEN_WIDTH - BG_BRICK_SIZE)) {
            draw_rect(rd.screen, &r, ++col);
            r.x += BG_BRICK_SIZE * 2;
        }
        r.x = 0;
        r.y += BG_BRICK_SIZE * 2;
    }
}

/*
static void animation_frames_init(void)
{
    int i;
    init_animation(&test_anim, 48, 32, 32);

    for(i = 0; i < test_anim.frame_count; ++i) {
        FILL_BUFFER(test_anim.frames[i], 18 + i);
    }
}*/

static void add_border(void)
{
    Rect btm = {0, 170, SCREEN_WIDTH, SCREEN_HEIGHT - 170};
    rd.screen_clipping.h = 170;
    draw_rect(rd.screen, &btm, 6);
}

static void bouncing_sprites_init(void)
{
    Rect rect = {0,0,16,16};
    int i;
    for(i = 3; i < SPRITE_COUNT - 1; ++i) {
        rect.x = rand() % 320;
        rect.y = rand() % 200;
        rd.sprites[i].rect = rect;
        rd.sprites[i].vis.colour = rand() % 255;
        rd.sprites[i].flags = SPRITE_REFRESH | SPRITE_CLIP | SPRITE_FILL;
        bounce_dirs[i].x = -1;
        bounce_dirs[i].y = 1;
    }
}

static void bouncing_sprites_update(void)
{
    int i;
    for(i = 3; i < SPRITE_COUNT - 1; ++i) {
        rd.sprites[i].rect.x += bounce_dirs[i].x;
        rd.sprites[i].rect.y += bounce_dirs[i].y;

        if(rd.sprites[i].rect.x >= SCREEN_WIDTH)
            bounce_dirs[i].x = -1;
        else if(rd.sprites[i].rect.x < 0)
            bounce_dirs[i].x = 1;

        if(rd.sprites[i].rect.y >= SCREEN_HEIGHT)
            bounce_dirs[i].y = -1;
        else if(rd.sprites[i].rect.y < 0)
            bounce_dirs[i].y = 1;
    }
}

static void update(void) {
    if(!paused) {
        bouncing_sprites_update();
        rd.sprites[1].rect.x += 1;
        if(rd.sprites[1].rect.x > SCREEN_WIDTH + 100)
            rd.sprites[1].rect.x = -100;

        rd.sprites[SPRITE_COUNT - 1].rect.y += 1;
        if(rd.sprites[SPRITE_COUNT - 1].rect.y > SCREEN_HEIGHT + 32)
            rd.sprites[SPRITE_COUNT - 1].rect.y = -32;
    }
}

static void render(void) {
    /*FILL_BUFFER(rd.screen, col++);
    */
    if(!paused) {
        start_frame(&rd);
        refresh_sprites(&rd);
    }
}

static bool input(void) {
    keyboard_per_frame_update();
    if(keyboard_os_quit_event())
        return false;
    
    return true;
}

static void quit(void) {
    quit_renderer(&rd);

    exit(1);
}

static void render_flip(void)
{
    finish_frame(&rd);
}

void test_keyboard(void)
{
/*
    int i;
    
    keyboard_init();
    

    while(!(keyboard_os_quit_event() || keyboard_keys[KEY_ESC])) {
    for(i = 0; i < KEYCODES_MAX; ++i) {
            if(keyboard_keys[i])
                printf("Key down: %d\n", i);
        }
        
        printf("\n");
    } */
    
//    keyboard_quit();
}

void unpause_callback(void *self)
{
    paused = false;
}

void done_eventing(void *self)
{
    paused = true;
    printf("Event fired\n");
    event_add(unpause_callback, self, 20);
    event_add(done_eventing, self, 200);
}

void should_not_fire(void *self)
{
    printf("This should not happen\n");
}

void test_start(bool do_benchmark, int benchmark_times)
{
    uint16 i;
    CoreData cd;
    EventID e;
    buffer_t *balloon_img;
    buffer_t *pal;

    cd.update_callback = &update;
    cd.render_callback = &render;
    cd.flip_callback = &render_flip;
    cd.input_handler = &input;
    cd.exit_handler = &quit;
    cd.frame_skip = 0;

    balloon_img = load_bmp_image("RES/BALLOON.BMP");
    pal = load_bmp_palette("RES/BALLOON.BMP");

    rd.flags |= RENDER_DOUBLE_BUFFER;

    if(!init_renderer(&rd, SPRITE_COUNT, pal))
        return;


    //rd.sprites[0].anim = &test_anim;
    rd.sprites[0].vis.colour = 4;
    rd.sprites[0].rect.w = 32;
    rd.sprites[0].rect.h = 32;
    rd.sprites[0].rect.x = 16;
    rd.sprites[0].rect.y = 64;
    rd.sprites[0].flags = SPRITE_REFRESH | SPRITE_FILL;

    rd.sprites[1].vis.image = create_image(32,32);

    for(i = 0; i < 32 * 32; ++i) {
        rd.sprites[1].vis.image[i] = i;
    }

    rd.sprites[1].vis.image = rd.sprites[1].vis.image;
    rd.sprites[1].rect.w = 32;
    rd.sprites[1].rect.h = 32;
    rd.sprites[1].rect.x = 128;
    rd.sprites[1].rect.y = 128;
    rd.sprites[1].flags = SPRITE_REFRESH | SPRITE_CLIP | SPRITE_MASKED;

    rd.sprites[2].vis.colour = 4;
    rd.sprites[2].rect.w = 8;
    rd.sprites[2].rect.h = 16;
    rd.sprites[2].rect.x = 24;
    rd.sprites[2].rect.y = -8;
    rd.sprites[2].flags = SPRITE_REFRESH | SPRITE_CLIP | SPRITE_FILL;
    rd.sprites[2].parent = &rd.sprites[1].rect;

    rd.sprites[SPRITE_COUNT - 1].vis.image = balloon_img;
    rd.sprites[SPRITE_COUNT - 1].rect.w = 32;
    rd.sprites[SPRITE_COUNT - 1].rect.h = 32;
    rd.sprites[SPRITE_COUNT - 1].rect.x = 256;
    rd.sprites[SPRITE_COUNT - 1].rect.y = 128;
    rd.sprites[SPRITE_COUNT - 1].flags = SPRITE_REFRESH | SPRITE_CLIP | SPRITE_MASKED;

    //animation_frames_init();
    bouncing_sprites_init();
    FILL_BUFFER(rd.screen, 3);
    FILL_BUFFER(rd.bg_layer, 3);
    add_bricks();
    add_border();

    e = event_add(should_not_fire, NULL, 700);
    event_add(done_eventing, NULL, 400);
    event_remove(e);

    //ASSERT(0);

    refresh_sprites(&rd);
    
    if(do_benchmark)
        engine_benchmark(cd, benchmark_times);
    else
        engine_start(cd);
}
