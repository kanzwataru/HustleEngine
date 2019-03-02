/* Poly TEST */
#include "engine/core.h"
#include "engine/render.h"
#include "platform/filesys.h"

static RenderData *rd;

static int geo_tris = 2;
static Point geo[] = {
    {300, 20},
    {300, 180},
    {20,  180},

    {20, 20},
    {300, 20},
    {20,  180},
};

static void update(void)
{
}

static void render(void)
{
    int i;
    renderer_start_frame(rd);

    for(i = 0; i < geo_tris * 3; i += 3) {
        draw_line_raw(rd->screen, geo[i + 0].x, geo[i + 0].y, geo[i + 1].x, geo[i + 1].y, i + 1);
        draw_line_raw(rd->screen, geo[i + 1].x, geo[i + 1].y, geo[i + 2].x, geo[i + 2].y, i + 2);
        draw_line_raw(rd->screen, geo[i + 2].x, geo[i + 2].y, geo[i + 0].x, geo[i + 0].y, i + 3);
    }
}

static void render_flip(void)
{
    renderer_finish_frame(rd);
}

static bool input(void)
{
    keyboard_per_frame_update();
    if(keyboard_os_quit_event() || keyboard_keys[KEY_ESC])
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
    buffer_t *pal;

    engine_init();

    cd.update_callback = &update;
    cd.render_callback = &render;
    cd.flip_callback = &render_flip;
    cd.input_handler = &input;
    cd.exit_handler = &quit;

    pal = load_bmp_palette("RES/VGAPAL.BMP");
    rd = renderer_init(0, RENDER_BG_SOLID, pal);
    rd->bg.colour = 1;
    destroy_image(&pal);

    engine_gameloop(cd);

    return 0;
}
