/* Poly TEST */
#include "engine/core.h"
#include "engine/render.h"
#include "engine/render/internal.h"
#include "platform/filesys.h"

#include "common/math3d.h"

#include <stdlib.h>
#include <stdio.h>

static RenderData *rd;

/*
static const int geo_tris = 2;
static const Vec3D geo[] = {
    {200, 20, 100},
    {200, 130, 100},
    {30,  180, 100},

    {30, 15, 100},
    {200, 10, 100},
    {10,  100, 100},
};

static const Vec3D smol[] = {
    {300, 80, 60},
    {310, 50, 60},
    {290, 70, 60}
};
*/

static const Vec3D cube_verts[] = {
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

static const Vec3D cube_normals[] = {
    { 0,  0, -1},
    { 0,  0, -1},
    { 0,  0, -1},
    { 0,  0, -1},
    { 0,  0, -1},
    { 0,  0, -1},

    { 0,  0, 1},
    { 0,  0, 1},
    { 0,  0, 1},
    { 0,  0, 1},
    { 0,  0, 1},
    { 0,  0, 1},

    {-1,  0,  0},
    {-1,  0,  0},
    {-1,  0,  0},
    {-1,  0,  0},
    {-1,  0,  0},
    {-1,  0,  0},

    { 1,  0,  0},
    { 1,  0,  0},
    { 1,  0,  0},
    { 1,  0,  0},
    { 1,  0,  0},
    { 1,  0,  0},

    { 0, -1,  0},
    { 0, -1,  0},
    { 0, -1,  0},
    { 0, -1,  0},
    { 0, -1,  0},
    { 0, -1,  0},

    { 0,  1,  0},
    { 0,  1,  0},
    { 0,  1,  0},
    { 0,  1,  0},
    { 0,  1,  0},
    { 0,  1,  0}
};

static Mesh cube;
static Triangle far cube_tris[12];
static uint16 rotation = 0;

static void update(void)
{
    rotation += 1;

    cube.xform.scale.x = 12;
    cube.xform.scale.y = 12;
    cube.xform.scale.z = 12;
    cube.xform.position.x = 0;
    cube.xform.position.y = (8) + (sin((float)rotation * 0.05f) * 8);
    cube.xform.position.z = 50;
    cube.xform.rotation.x = rotation;
    cube.xform.rotation.y = rotation;
    cube.xform.rotation.z = 0;
}

/*
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
} */
/*
static void draw_tris(buffer_t *buf, Vec3D *geo, int tris)
{
    int i, x, y;
    float segment_height;
    float total_height;
    float alpha;
    float beta;
    Vec3D a;
    Vec3D b;
    Vec3D tri[3];

    for(i = 0; i < tris * 3; i += 3) {
        if(geo[i + 2].z > 0 || geo[i + 1].z > 0 || geo[i + 0].z > 0)
            continue;

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

            a.x = CLAMP(a.x, 0, 320);
            a.y = CLAMP(a.y, 0, 200);

            b.x = CLAMP(b.x, 0, 320);
            b.y = CLAMP(b.y, 0, 200);

            for(x = a.x; x <= b.x; ++x) {
                if(x <= 0 || x >= 320 || y <= 0 || y >= 200)
                    continue;

                buf[CALC_OFFSET(x, y)] = i + 32;
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

            a.x = CLAMP(a.x, 0, 320);
            a.y = CLAMP(a.y, 0, 200);

            b.x = CLAMP(b.x, 0, 320);
            b.y = CLAMP(b.y, 0, 200);

            for(x = a.x; x <= b.x; ++x) {
                if(x <= 0 || x >= 320 || y <= 0 || y >= 200)
                    continue;

                buf[CALC_OFFSET(x, y)] = i + 32;
            }
        }
    }
}
*/
int zsort(const void *a, const void *b) {
    int average_a = (((Vec3D*)a)[0].z + ((Vec3D*)a)[1].z + ((Vec3D*)a)[2].z) / 3;
    int average_b = (((Vec3D*)b)[0].z + ((Vec3D*)b)[1].z + ((Vec3D*)b)[2].z) / 3;

    return average_b < average_a;
}

static void render(void)
{
    int i;
    /*
    Vec4D tmp;
    Vec3D geo_xformed[12 * 3];
    Matrix model = MAT_IDENTITY;
    Matrix proj;
    Matrix xform = MAT_IDENTITY;

    Vec3D rotation_axis = {1.0f, 1.0f, 0.0f};
    Vec3D scale = {12, 12, 12};
    Vec3D translation;
    translation.x = 0;
    translation.y = (8) + (sin((float)rotation * 0.05f) * 8);
    translation.z = 50;

    mat_perspective_make(proj, RADIANS(90), 240.0f / 200.0f, 0.1f, 1000.0f);
    mat_translate(model, translation);
    mat_rotate(model, RADIANS(rotation), rotation_axis);
    mat_scale(model, scale);

    mat_mul_mat(xform, model, proj);

    for(i = 0; i < 12 * 3; ++i) {
        tmp[0] = cube[i].x;
        tmp[1] = cube[i].y;
        tmp[2] = cube[i].z;
        tmp[3] = 1.0f;

        mat_mul_vec(tmp, xform);

        if(tmp[3] == 0)
            tmp[3] = 0.001f;

        tmp[0] /= tmp[3];     tmp[1] /= tmp[3];       // persp divide

        tmp[0] += 1.0f;       tmp[1] += 1.0f;         // put -1:1 to 0:2
        tmp[0] *= 0.5f * 320; tmp[1] *= 0.5f * 240;   // scale to screen;

        geo_xformed[i].x = (int)tmp[0];
        geo_xformed[i].y = (int)tmp[1];
        geo_xformed[i].z = (int)tmp[2];
    }

    qsort(geo_xformed, 12, 3 * sizeof(Vec3D), zsort);

    renderer_start_frame(rd);
    FILL_BUFFER(rd->screen, 0);
    //draw_tris(rd->screen, geo, geo_tris);
    //draw_tris(rd->screen, smol, 1);
    draw_tris(rd->screen, geo_xformed, 12);
    //draw_tris_wire(rd->screen, geo_xformed, 12);

    //draw_tris_wire(rd->screen, geo, geo_tris);
    //draw_tris_wire(rd->screen, smol, 1);
    */
    renderer_start_frame(rd);
    FILL_BUFFER(rd->screen, 0);
    draw_mesh(rd->screen, &cube);
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
    int i;
    CoreData cd;
    buffer_t *pal;

    engine_init();


    cd.update_callback = &update;
    cd.render_callback = &render;
    cd.flip_callback = &render_flip;
    cd.input_handler = &input;
    cd.exit_handler = &quit;

    // load palette
    pal = load_bmp_palette("RES/3DPAL.BMP");
    rd = renderer_init(0, RENDER_BG_SOLID, pal);
    rd->bg.colour = 1;
    destroy_image(&pal);

    // set up cube mesh
    cube.tri_count = 12;
    cube.tris = cube_tris;
    for(i = 0; i < cube.tri_count; ++i) {
        cube.tris[i].vertices[0] = cube_verts[(i * 3) + 0];
        cube.tris[i].vertices[1] = cube_verts[(i * 3) + 1];
        cube.tris[i].vertices[2] = cube_verts[(i * 3) + 2];

        cube.tris[i].normals[0] = cube_normals[(i * 3) + 0];
        cube.tris[i].normals[1] = cube_normals[(i * 3) + 1];
        cube.tris[i].normals[2] = cube_normals[(i * 3) + 2];

        cube.tris[i].color = 0x7F;
    }

    engine_gameloop(cd);

    return 0;
}
