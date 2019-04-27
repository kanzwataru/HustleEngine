#include "common/platform.h"
#include "engine/core.h"
#include "engine/render.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static RenderData *rd;
static memid_t tblock = 0;
static void far *transientmem = NULL;

static bool input(void)
{
    keyboard_per_frame_update();
    if(keyboard_os_quit_event())
        return false;

    return true;
}

static void update(void)
{
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
    mem_free_block(tblock);
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
    tblock = mem_alloc_block(MEMSLOT_RENDERER_TRANSIENT);
    transientmem = mem_slot_get(tblock);

    srand(time(NULL));
    for(i = 0; i < 256; ++i) {
        palette[i] = rand();
    }

    rd = renderer_init(transientmem, 0, 0, palette);

    while(input()) {
        update();
        render();
        render_flip();
    }

    engine_quit();

    quit();
}
