#include "common/platform.h"
#include "engine/core.h"
#include "engine/render.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static RenderData *rd;

static bool input(void)
{
    keyboard_per_frame_update();
    if(keyboard_os_quit_event())
        return false;
    
    return true;
}

static void update(void)
{
    srand(time(NULL));
    printf("%d\n", rand());
}

static void render(void)
{
    int i;
    renderer_start_frame(rd);
    
    for(i = 0; i < 320 * 200; ++i) {
        rd->screen[i] = rand();
    }
}

static void quit(void)
{
    renderer_quit(rd, true);
}

static void render_flip(void)
{
    renderer_finish_frame(rd);
}

void simpletest_start(void)
{
    int i;
    byte palette[256];
    
    engine_init();
    
    srand(time(NULL));
    for(i = 0; i < 256; ++i) {
        palette[i] = rand();
    }
    
    rd = renderer_init(0, &palette[0]);
    
    while(input()) {
        update();
        render();
        render_flip();
    }
    
    engine_quit();
    
    quit();
}
