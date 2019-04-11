/*
 * Largely untested vector math library
 *
 * This could all be hopelessly broken
*/
#ifndef VECTOR_H
#define VECTOR_H

#include "common/prim.h"
#include "common/math.h"
#include <math.h>

/*
 * Matrix math
*/
#ifdef USE_MATRIX

#define MM(A, B) (((A) * 4) + (B))

static inline Matrix midentity(void) {
    Matrix m = {{1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1}};

    return m;
}

static inline Matrix mzero(void) {
    Matrix m = {{0, 0, 0, 0,
                 0, 0, 0, 0,
                 0, 0, 0, 0,
                 0, 0, 0, 0}};

    return m;
}

static inline Matrix mmulm(const Matrix *a, const Matrix *b) {
    Matrix m;
    m.m[0] = a->m[0] * b->m[0] + a->m[1] * b->m[4] + a->m[2] * b->m[8]  + a->m[3] * b->m[12];
    m.m[1] = a->m[0] * b->m[1] + a->m[1] * b->m[5] + a->m[2] * b->m[9]  + a->m[3] * b->m[13];
    m.m[2] = a->m[0] * b->m[2] + a->m[1] * b->m[6] + a->m[2] * b->m[10] + a->m[3] * b->m[14];
    m.m[3] = a->m[0] * b->m[3] + a->m[1] * b->m[7] + a->m[2] * b->m[11] + a->m[3] * b->m[15];

    m.m[4] = a->m[4] * b->m[0] + a->m[5] * b->m[4] + a->m[6] * b->m[8]  + a->m[7] * b->m[12];
    m.m[5] = a->m[4] * b->m[1] + a->m[5] * b->m[5] + a->m[6] * b->m[9]  + a->m[7] * b->m[13];
    m.m[6] = a->m[4] * b->m[2] + a->m[5] * b->m[6] + a->m[6] * b->m[10] + a->m[7] * b->m[14];
    m.m[7] = a->m[4] * b->m[3] + a->m[5] * b->m[7] + a->m[6] * b->m[11] + a->m[7] * b->m[15];

    m.m[8]  = a->m[8] * b->m[0] + a->m[9] * b->m[4] + a->m[10] * b->m[8]  + a->m[11] * b->m[12];
    m.m[9]  = a->m[8] * b->m[1] + a->m[9] * b->m[5] + a->m[10] * b->m[9]  + a->m[11] * b->m[13];
    m.m[10] = a->m[8] * b->m[2] + a->m[9] * b->m[6] + a->m[10] * b->m[10] + a->m[11] * b->m[14];
    m.m[11] = a->m[8] * b->m[3] + a->m[9] * b->m[7] + a->m[10] * b->m[11] + a->m[11] * b->m[15];

    m.m[12] = a->m[12] * b->m[0] + a->m[13] * b->m[4] + a->m[14] * b->m[8]  + a->m[15] * b->m[12];
    m.m[13] = a->m[12] * b->m[1] + a->m[13] * b->m[5] + a->m[14] * b->m[9]  + a->m[15] * b->m[13];
    m.m[14] = a->m[12] * b->m[2] + a->m[13] * b->m[6] + a->m[14] * b->m[10] + a->m[15] * b->m[14];
    m.m[15] = a->m[12] * b->m[3] + a->m[13] * b->m[7] + a->m[14] * b->m[11] + a->m[15] * b->m[15];

    return m;
}

static inline Vec3Df mmulv3(const Matrix *m, const Vec3Df *v) {
    Vec3Df r;
    r.x = m->m[0] * v->x + m->m[4] * v->y + m->m[8]  * v->z + m->m[12];
    r.y = m->m[1] * v->x + m->m[5] * v->y + m->m[9]  * v->z + m->m[13];
    r.z = m->m[2] * v->x + m->m[6] * v->y + m->m[10] * v->z + m->m[14];

    return r;
}

static inline Matrix mperspective(float fov_y, float aspect, float near_clip, float far_clip) {
    float fov;
    float far_near;
    Matrix m = mzero();

    fov = 1.0f / tan(fov_y * 0.5f);
    far_near = 1.0f / (near_clip - far_clip);

    m.m[MM(0,0)] = fov / aspect;
    m.m[MM(1,1)] = fov;
    m.m[MM(2,2)] = (near_clip + far_clip) * far_near;
    m.m[MM(2,3)] = -1.0f;
    m.m[MM(3,2)] = 2.0f * near_clip * far_clip * far_near;

    return m;
}

#endif /* USE_MATRIX */

/*
 * Reusable template macros *
*/
#define DECLARE_2DOP(NAME, T, OP) \
static inline T NAME(T a, T b) {  \
    T v;                          \
    v.x = a.x OP b.x;             \
    v.y = a.y OP b.y;             \
    return v;                     \
}

#define DECLARE_2DSCALAROP(NAME, TV, TS, OP) \
static inline TV NAME(TV a, TS b) {  \
    TV v;                            \
    v.x = a.x OP b;                  \
    v.y = a.y OP b;                  \
    return v;                        \
}

#define DECLARE_3DOP(NAME, T, OP) \
static inline T NAME(T a, T b) {  \
    T v;                          \
    v.x = a.x OP b.x;             \
    v.y = a.y OP b.y;             \
    v.z = a.z OP b.z;             \
    return v;                     \
}

#define DECLARE_3DSCALAROP(NAME, TV, TS, OP) \
static inline TV NAME(TV a, TS b) {  \
    TV v;                            \
    v.x = a.x OP b;                  \
    v.y = a.y OP b;                  \
    v.z = a.z OP b;                  \
    return v;                        \
}

/*
 * 2D Int Vector
*/
#ifdef USE_2DVEC_INT
    DECLARE_2DOP(pmulp, Point, *)
    DECLARE_2DOP(pdivp, Point, /)
    DECLARE_2DOP(paddp, Point, +)
    DECLARE_2DOP(psubp, Point, -)

    DECLARE_2DSCALAROP(pmuli, Point, int, *)
    DECLARE_2DSCALAROP(pdivi, Point, int, /)
    DECLARE_2DSCALAROP(paddi, Point, int, +)
    DECLARE_2DSCALAROP(psubi, Point, int, -)
    DECLARE_2DSCALAROP(pshr,  Point, int, >>)
    DECLARE_2DSCALAROP(pshl,  Point, int, <<)
#endif

/*
 * 2D float Vector
*/
#ifdef USE_2DVEC_FLOAT
    DECLARE_2DOP(pfmulpf, Pointf, *)
    DECLARE_2DOP(pfdivpf, Pointf, /)
    DECLARE_2DOP(pfaddpf, Pointf, +)
    DECLARE_2DOP(pfsubpf, Pointf, -)

    DECLARE_2DSCALAROP(pfmulf, Pointf, float, *)
    DECLARE_2DSCALAROP(pfdivf, Pointf, float, /)
    DECLARE_2DSCALAROP(pfaddf, Pointf, float, +)
    DECLARE_2DSCALAROP(pfsubf, Pointf, float, -)
#endif

/*
 * 3D Int Vector
*/
#ifdef USE_3DVEC_INT
    DECLARE_3DOP(v3mul, Vec3D, *)
    DECLARE_3DOP(v3div, Vec3D, /)
    DECLARE_3DOP(v3add, Vec3D, +)
    DECLARE_3DOP(v3sub, Vec3D, -)

    DECLARE_3DSCALAROP(v3muli, Vec3D, int, *)
    DECLARE_3DSCALAROP(v3divi, Vec3D, int, /)
    DECLARE_3DSCALAROP(v3addi, Vec3D, int, +)
    DECLARE_3DSCALAROP(v3subi, Vec3D, int, -)
    DECLARE_3DSCALAROP(v3shr,  Vec3D, int, >>)
    DECLARE_3DSCALAROP(v3shl,  Vec3D, int, <<)

    DECLARE_3DSCALAROP(v3mulf, Vec3D, float, *)
    DECLARE_3DSCALAROP(v3divf, Vec3D, float, /)
    DECLARE_3DSCALAROP(v3addf, Vec3D, float, +)
    DECLARE_3DSCALAROP(v3subf, Vec3D, float, -)
#endif
/*
 * 3D Float Vector
*/
#ifdef USE_3DVEC_FLOAT
    DECLARE_3DOP(v3fmul, Vec3Df, *)
    DECLARE_3DOP(v3fdiv, Vec3Df, /)
    DECLARE_3DOP(v3fadd, Vec3Df, +)
    DECLARE_3DOP(v3fsub, Vec3Df, -)

    DECLARE_3DSCALAROP(v3fmuli, Vec3Df, int, *)
    DECLARE_3DSCALAROP(v3fdivi, Vec3Df, int, /)
    DECLARE_3DSCALAROP(v3faddi, Vec3Df, int, +)
    DECLARE_3DSCALAROP(v3fsubi, Vec3Df, int, -)

    DECLARE_3DSCALAROP(v3fmulf, Vec3Df, float, *)
    DECLARE_3DSCALAROP(v3fdivf, Vec3Df, float, /)
    DECLARE_3DSCALAROP(v3faddf, Vec3Df, float, +)
    DECLARE_3DSCALAROP(v3fsubf, Vec3Df, float, -)
#endif

#endif /* VECTOR_H */
