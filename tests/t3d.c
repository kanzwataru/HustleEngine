/* Poly TEST */
#include "engine/core.h"
#include "engine/render.h"

static RenderData *rd;

static void update(void)
{

}

static void render(void)
{
    renderer_start_frame(rd);
}

static void render_flip(void)
{
    renderer_finish_frame(rd);
}

static bool input(void)
{
    keyboard_per_frame_update();
    if(keyboard_os_quit_event() || keyboard_keys[KEY_LEFT])
        return false;

    return true;
}

static void quit(void)
{
    renderer_quit(rd, true);
    engine_quit();
}

int polytest_start(void)
{
    CoreData cd;

    engine_init();

    cd.update_callback = &update;
    cd.render_callback = &render;
    cd.flip_callback = &render_flip;
    cd.input_handler = &input;
    cd.exit_handler = &quit;

    rd = renderer_init(1, RENDER_BG_SOLID, NULL);
    rd->bg.colour = 1;

    engine_gameloop(cd);

    return 0;
}
