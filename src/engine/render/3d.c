#include "internal.h"
#include "platform/mem.h"
#include "engine/render.h"
#include "engine/render/3d.h"
#include "common/math3d.h"

#define DEFINE_SWAP(name, type) static void swap_##name (type *a, type *b) { \
    type c = *a;    \
    *a = *b;        \
    *b = c;         \
}

DEFINE_SWAP(vec, Vec3D)
DEFINE_SWAP(point, Point)

static const Vec3D x = {1.0f, 0.0f, 0.0f};
static const Vec3D y = {0.0f, 1.0f, 0.0f};
static const Vec3D z = {0.0f, 0.0f, 1.0f};

static int zsort(const void *a, const void *b) {
    int average_a = (((Triangle far *)a)->vertices[0].z + ((Triangle far *)a)->vertices[1].z + ((Triangle far *)a)->vertices[2].z) / 3;
    int average_b = (((Triangle far *)b)->vertices[0].z + ((Triangle far *)b)->vertices[1].z + ((Triangle far *)b)->vertices[2].z) / 3;

    return average_a > average_b;
}

void draw_tris_wire(buffer_t *buf, Triangle far *tris, size_t count)
{
    int i;

    for(i = 0; i < count; ++i) {
        draw_line_raw(buf, tris[i].vertices[0].x, tris[i].vertices[0].y,
                           tris[i].vertices[1].x, tris[i].vertices[1].y,
                           tris[i].color);
        draw_line_raw(buf, tris[i].vertices[1].x, tris[i].vertices[1].y,
                           tris[i].vertices[2].x, tris[i].vertices[2].y,
                           tris[i].color);
        draw_line_raw(buf, tris[i].vertices[1].x, tris[i].vertices[1].y,
                           tris[i].vertices[2].x, tris[i].vertices[2].y,
                           tris[i].color);
    }
}

void draw_tris(buffer_t *buf, Triangle far *tris, size_t count)
{
    int   i, x, y;
    bool  second_half;
    float segment_height;
    float total_height;
    float alpha;
    float beta;
    Point a;
    Point b;
    Vec3D tri[3];

    for(i = 0; i < count; ++i) {
        //printf("[%d] (%f %f %f)\n", i, tris[i].vertices[0].x, tris[i].vertices[0].y, tris[i].vertices[0].z);
        //printf("[%d] (%f %f %f)\n", i, tris[i].vertices[1].x, tris[i].vertices[1].y, tris[i].vertices[1].z);
        //printf("[%d] (%f %f %f)\n", i, tris[i].vertices[2].x, tris[i].vertices[2].y, tris[i].vertices[2].z);

        if(tris[i].vertices[2].z > 0 || tris[i].vertices[1].z > 0 || tris[i].vertices[0].z > 0 /* skip behind camera geo */
           || (tris[i].vertices[0].y == tris[i].vertices[1].y && tris[i].vertices[0].y == tris[i].vertices[2].y))     /* skip degenarate tris */
            continue;

        tri[0] = tris[i].vertices[0];
        tri[1] = tris[i].vertices[1];
        tri[2] = tris[i].vertices[2];
        if(tri[0].y > tri[1].y) swap_vec(&tri[0], &tri[1]);
        if(tri[0].y > tri[2].y) swap_vec(&tri[0], &tri[2]);
        if(tri[1].y > tri[2].y) swap_vec(&tri[1], &tri[2]);

        total_height = tri[2].y - tri[0].y;
        for(y = tri[0].y; y < tri[1].y; ++y) {
            segment_height = tri[1].y - tri[0].y + 1;

            alpha = (float)(y - tri[0].y) / total_height;
            beta = (float)(y - tri[0].y) / segment_height;

            a.x = tri[0].x + (tri[2].x - tri[0].x) * alpha;
            a.y = tri[0].y + (tri[2].y - tri[0].y) * alpha;

            b.x = tri[0].x + (tri[1].x - tri[0].x) * beta;
            b.y = tri[0].y + (tri[1].y - tri[0].y) * beta;

            if(a.x > b.x) swap_point(&a, &b);

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

            if(a.x > b.x) swap_point(&a, &b);

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

void draw_mesh(buffer_t *buf, Mesh *mesh)
{
    int i, j;

    Vec4D tmp;
    Matrix model = MAT_IDENTITY;
    Matrix proj;
    Matrix xform;

    /* TODO: 現状2Dレンダラーのメモリーを踏んでいるので
           　スプライト描画を壊れないように修正したい
    */
    Triangle far *transformed;
    void far *transientmem = mem_slot_get(MEMSLOT_RENDERER_TRANSIENT);
    transformed = (Triangle far *)transientmem;
    _fmemcpy(transformed, mesh->tris, mesh->tri_count * sizeof(Triangle));
    /* */

    mat_perspective_make(proj, RADIANS(90), 240.0f / 200.0f, 0.1f, 1000.0f); //カメラでやったほうがいい
    mat_translate(model, mesh->xform.position);
    mat_rotate(model, RADIANS(mesh->xform.rotation.x), x);
    mat_rotate(model, RADIANS(mesh->xform.rotation.y), y);
    mat_rotate(model, RADIANS(mesh->xform.rotation.z), z);
    mat_scale(model, mesh->xform.scale);

    mat_mul_mat(xform, model, proj);

    for(i = 0; i < mesh->tri_count; ++i) {
        for(j = 0; j < 3; ++j) {
            tmp[0] = mesh->tris[i].vertices[j].x;
            tmp[1] = mesh->tris[i].vertices[j].y;
            tmp[2] = mesh->tris[i].vertices[j].z;
            tmp[3] = 1.0f;

            printf("[%d][%d] (%f %f %f) -> ", i, j, tmp[0], tmp[1], tmp[2]);

            mat_mul_vec(tmp, xform);

            printf("(%f %f %f) -> ", tmp[0], tmp[1], tmp[2]);

            if(tmp[3] == 0)
                tmp[3] = 0.001f;

            tmp[0] /= tmp[3];     tmp[1] /= tmp[3];       // persp divide

            tmp[0] += 1.0f;       tmp[1] += 1.0f;         // put -1:1 to 0:2
            tmp[0] *= 0.5f * 320; tmp[1] *= 0.5f * 240;   // scale to screen;

            printf("(%f %f %f)\n", tmp[0], tmp[1], tmp[2]);

            transformed[i].vertices[j].x = (int)tmp[0];
            transformed[i].vertices[j].y = (int)tmp[1];
            transformed[i].vertices[j].z = (int)tmp[2];

            //printf("(%f %f %f)\n", tmp[0], tmp[1], tmp[2]);
        }
    }

    qsort(transformed, mesh->tri_count, sizeof(Triangle), zsort);
    draw_tris(buf, transformed, mesh->tri_count);
}
