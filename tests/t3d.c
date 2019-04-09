/* Poly TEST */
#include "engine/core.h"
#include "engine/render.h"
#include "engine/render/internal.h"
#include "platform/filesys.h"
#include "common/vector.h"

static RenderData *rd;

static int geo_tris = 2;
static Point geo[] = {
    {200, 20},
    {200, 130},
    {30,  180},

    {30, 15},
    {200, 10},
    {10,  100},
};

static Point smol[] = {
    {300, 80},
    {310, 50},
    {290, 70}
};

static void update(void)
{
}

static void draw_tris_wire(buffer_t *buf, Point *geo, int tris)
{
    int i;

    for(i = 0; i < tris * 3; i += 3) {
        draw_line_raw(rd->screen, geo[i + 0].x, geo[i + 0].y, geo[i + 1].x, geo[i + 1].y, i + 1);
        draw_line_raw(rd->screen, geo[i + 1].x, geo[i + 1].y, geo[i + 2].x, geo[i + 2].y, i + 2);
        draw_line_raw(rd->screen, geo[i + 2].x, geo[i + 2].y, geo[i + 0].x, geo[i + 0].y, i + 3);
    }
}

static void swap(Point *a, Point *b) {
    Point c = *a;
    *a = *b;
    *b = c;
}

static void draw_tris(buffer_t *buf, Point *geo, int tris)
{
    int i, x, y;
    Point tri[3];

    for(i = 0; i < tris * 3; i += 3) {
        memcpy(tri, geo + i, 3 * sizeof(Point));
        if(tri[0].y > tri[1].y) swap(&tri[0], &tri[1]);
        if(tri[0].y > tri[2].y) swap(&tri[0], &tri[2]);
        if(tri[1].y > tri[2].y) swap(&tri[1], &tri[2]);

        int total_height = tri[2].y - tri[0].y;
        for(y = tri[0].y; y < tri[1].y; ++y) {
            int segment_height = tri[1].y - tri[0].y + 1;

            float alpha = (float)(y - tri[0].y) / total_height;
            float beta = (float)(y - tri[0].y) / segment_height;

            Point a = {
                tri[0].x + (tri[2].x - tri[0].x) * alpha,
                tri[0].y + (tri[2].y - tri[0].y) * alpha
            };

            Point b = {
                tri[0].x + (tri[1].x - tri[0].x) * beta,
                tri[0].y + (tri[1].y - tri[0].y) * beta
            };

            if(a.x > b.x) swap(&a, &b);
            for(x = a.x; x <= b.x; ++x) {
                buf[CALC_OFFSET(x, y)] = 3;
            }
        }
        for(y = tri[1].y; y < tri[2].y; ++y) {
            int segment_height = tri[2].y - tri[1].y + 1;

            float alpha = (float)(y - tri[0].y) / total_height;
            float beta = (float)(y - tri[1].y) / segment_height;

            Point a = {
                tri[0].x + (tri[2].x - tri[0].x) * alpha,
                tri[0].y + (tri[2].y - tri[0].y) * alpha
            };

            Point b = {
                tri[1].x + (tri[2].x - tri[1].x) * beta,
                tri[1].y + (tri[2].y - tri[1].y) * beta
            };

            if(a.x > b.x) swap(&a, &b);
            for(x = a.x; x <= b.x; ++x) {
                buf[CALC_OFFSET(x, y)] = 3;
            }
        }

/*
        draw_line_raw(rd->screen, tri[0].x, tri[0].y, tri[1].x, tri[1].y, 2);
        draw_line_raw(rd->screen, tri[1].x, tri[1].y, tri[2].x, tri[2].y, 2);
        draw_line_raw(rd->screen, tri[2].x, tri[2].y, tri[0].x, tri[0].y, 5);
*/
    }
}


static void render(void)
{
    renderer_start_frame(rd);
    draw_tris(rd->screen, geo, geo_tris);
    draw_tris(rd->screen, smol, 1);

    //draw_tris_wire(rd->screen, geo, geo_tris);
    //draw_tris_wire(rd->screen, smol, 1);
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
