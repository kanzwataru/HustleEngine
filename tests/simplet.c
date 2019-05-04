#include "common/platform.h"
#include "engine/core.h"
#include "engine/render.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static RenderData *rd;
static void *transientmem = NULL;

static bool input(void)
{
    if(keyboard_os_quit_event() || keyboard_keys[KEY_ESC])
        return false;

    return true;
}

static void update(void)
{
    DEBUG_DO(printf("%d\n", rand()));
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
    transientmem = malloc(64000);

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
