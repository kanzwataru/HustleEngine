/* Poly TEST */
#include "engine/core.h"
#include "engine/render.h"
#include "engine/render/internal.h"
#include "platform/filesys.h"

#include "extern/cglm/mat4.h"
#include "extern/cglm/vec4.h"
#include "extern/cglm/affine.h"

#define USE_MATRIX
#define USE_3DVEC_INT
#define USE_3DVEC_FLOAT
#include "common/vector.h"

static RenderData *rd;

static int geo_tris = 2;
static Vec3D geo[] = {
    {200, 20, 100},
    {200, 130, 100},
    {30,  180, 100},

    {30, 15, 100},
    {200, 10, 100},
    {10,  100, 100},
};

static Vec3D smol[] = {
    {300, 80, 60},
    {310, 50, 60},
    {290, 70, 60}
};

static Vec3D cube[] = {
    {-1, -1, -1},
    { 1, -1, -1},
    { 1,  1, -1},
    { 1,  1, -1},
    {-1,  1, -1},
    {-1, -1, -1},

    {-1, -1,  1},
    { 1, -1,  1},
    { 1,  1,  1},
    { 1,  1,  1},
    {-1,  1,  1},
    {-1, -1,  1},

    {-1,  1,  1},
    {-1,  1, -1},
    {-1, -1, -1},
    {-1, -1, -1},
    {-1, -1,  1},
    {-1,  1,  1},

    { 1,  1,  1},
    { 1,  1, -1},
    { 1, -1, -1},
    { 1, -1, -1},
    { 1, -1,  1},
    { 1,  1,  1},

    {-1, -1, -1},
    { 1, -1, -1},
    { 1, -1,  1},
    { 1, -1,  1},
    {-1, -1,  1},
    {-1, -1, -1},

    {-1,  1, -1},
    { 1,  1, -1},
    { 1,  1,  1},
    { 1,  1,  1},
    {-1,  1,  1},
    {-1,  1, -1}
};

static unsigned int rotation = 0;

static void update(void)
{
    rotation += 1;
}

static void draw_tris_wire(buffer_t *buf, Vec3D *geo, int tris)
{
    int i;

    for(i = 0; i < tris * 3; i += 3) {
        draw_line_raw(rd->screen, geo[i + 0].x, geo[i + 0].y, geo[i + 1].x, geo[i + 1].y, i + 1);
        draw_line_raw(rd->screen, geo[i + 1].x, geo[i + 1].y, geo[i + 2].x, geo[i + 2].y, i + 2);
        draw_line_raw(rd->screen, geo[i + 2].x, geo[i + 2].y, geo[i + 0].x, geo[i + 0].y, i + 3);
    }
}

static void swap(Vec3D *a, Vec3D *b) {
    Vec3D c = *a;
    *a = *b;
    *b = c;
}

static void draw_tris(buffer_t *buf, Vec3D *geo, int tris)
{
    int i, x, y;
    int segment_height;
    int total_height;
    float alpha;
    float beta;
    Vec3D a;
    Vec3D b;
    Vec3D tri[3];

    for(i = 0; i < tris * 3; i += 3) {
        memcpy(tri, geo + i, 3 * sizeof(Vec3D));
        if(tri[0].y > tri[1].y) swap(&tri[0], &tri[1]);
        if(tri[0].y > tri[2].y) swap(&tri[0], &tri[2]);
        if(tri[1].y > tri[2].y) swap(&tri[1], &tri[2]);

        total_height = tri[2].y - tri[0].y;
        for(y = tri[0].y; y < tri[1].y; ++y) {
            segment_height = tri[1].y - tri[0].y + 1;

            alpha = (float)(y - tri[0].y) / total_height;
            beta = (float)(y - tri[0].y) / segment_height;

            a.x = tri[0].x + (tri[2].x - tri[0].x) * alpha;
            a.y = tri[0].y + (tri[2].y - tri[0].y) * alpha;

            b.x = tri[0].x + (tri[1].x - tri[0].x) * beta;
            b.y = tri[0].y + (tri[1].y - tri[0].y) * beta;

            if(a.x > b.x) swap(&a, &b);
            for(x = a.x; x <= b.x; ++x) {
                buf[CALC_OFFSET(x, y)] = i + 1;
            }
        }
        for(y = tri[1].y; y < tri[2].y; ++y) {
            segment_height = tri[2].y - tri[1].y + 1;

            alpha = (float)(y - tri[0].y) / total_height;
            beta = (float)(y - tri[1].y) / segment_height;

            a.x = tri[0].x + (tri[2].x - tri[0].x) * alpha;
            a.y = tri[0].y + (tri[2].y - tri[0].y) * alpha;

            b.x = tri[1].x + (tri[2].x - tri[1].x) * beta;
            b.y = tri[1].y + (tri[2].y - tri[1].y) * beta;

            if(a.x > b.x) swap(&a, &b);
            for(x = a.x; x <= b.x; ++x) {
                if(x < 0 || x > 320 || y < 0 || y > 200)
                    continue;

                buf[CALC_OFFSET(x, y)] = i + 1;
            }
        }
    }
}

static void render(void)
{
    int i, j;

    vec4 in;
    vec4 out;
    Vec3D geo_xformed[12 * 3];
    mat4 model = GLM_MAT4_IDENTITY_INIT;

    glm_translate(model, (vec3){320 / 2, 200 / 2, 0});
    glm_rotate_at(model, (vec3){0.0f,0.0f,0.0f}, rotation * M_PI / 180, (vec3){1.0f, 1.0f, 0.0f});
    glm_scale_uni(model, 24);

    for(i = 0; i < 12 * 3; ++i) {
        in[0] = cube[i].x;
        in[1] = cube[i].y;
        in[2] = cube[i].z;
        in[3] = 1.0f;

        glm_mat4_mulv(model, in, out);

        geo_xformed[i].x = out[0] / out[3];
        geo_xformed[i].y = out[1] / out[3];
        geo_xformed[i].z = out[2] / out[3];
    }

    renderer_start_frame(rd);
    FILL_BUFFER(rd->screen, 0);
    //draw_tris(rd->screen, geo, geo_tris);
    //draw_tris(rd->screen, smol, 1);
    draw_tris(rd->screen, geo_xformed, 12);

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
