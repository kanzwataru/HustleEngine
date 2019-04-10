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
    DECLARE_2DOP(pmulp, Point, *);
    DECLARE_2DOP(pdivp, Point, /);
    DECLARE_2DOP(paddp, Point, +);
    DECLARE_2DOP(psubp, Point, -);

    DECLARE_2DSCALAROP(pmuli, Point, int, *);
    DECLARE_2DSCALAROP(pdivi, Point, int, /);
    DECLARE_2DSCALAROP(paddi, Point, int, +);
    DECLARE_2DSCALAROP(psubi, Point, int, -);
    DECLARE_2DSCALAROP(pshr,  Point, int, >>);
    DECLARE_2DSCALAROP(pshl,  Point, int, <<);
#endif

/*
 * 2D float Vector
*/
#ifdef USE_2DVEC_FLOAT
    DECLARE_2DOP(pfmulpf, Pointf, *);
    DECLARE_2DOP(pfdivpf, Pointf, /);
    DECLARE_2DOP(pfaddpf, Pointf, +);
    DECLARE_2DOP(pfsubpf, Pointf, -);

    DECLARE_2DSCALAROP(pfmulf, Pointf, float, *);
    DECLARE_2DSCALAROP(pfdivf, Pointf, float, /);
    DECLARE_2DSCALAROP(pfaddf, Pointf, float, +);
    DECLARE_2DSCALAROP(pfsubf, Pointf, float, -);
#endif

/*
 * 3D Int Vector
*/
#ifdef USE_3DVEC_INT
    DECLARE_3DOP(v3mul, Vec3D, *);
    DECLARE_3DOP(v3div, Vec3D, /);
    DECLARE_3DOP(v3add, Vec3D, +);
    DECLARE_3DOP(v3sub, Vec3D, -);

    DECLARE_3DSCALAROP(v3muli, Vec3D, int, *);
    DECLARE_3DSCALAROP(v3divi, Vec3D, int, /);
    DECLARE_3DSCALAROP(v3addi, Vec3D, int, +);
    DECLARE_3DSCALAROP(v3subi, Vec3D, int, -);
    DECLARE_3DSCALAROP(v3shr,  Vec3D, int, >>);
    DECLARE_3DSCALAROP(v3shl,  Vec3D, int, <<);

    DECLARE_3DSCALAROP(v3mulf, Vec3D, float, *);
    DECLARE_3DSCALAROP(v3divf, Vec3D, float, /);
    DECLARE_3DSCALAROP(v3addf, Vec3D, float, +);
    DECLARE_3DSCALAROP(v3subf, Vec3D, float, -);
#endif
/*
 * 3D Float Vector
*/
#ifdef USE_3DVEC_FLOAT
    DECLARE_3DOP(v3fmul, Vec3Df, *);
    DECLARE_3DOP(v3fdiv, Vec3Df, /);
    DECLARE_3DOP(v3fadd, Vec3Df, +);
    DECLARE_3DOP(v3fsub, Vec3Df, -);

    DECLARE_3DSCALAROP(v3fmuli, Vec3Df, int, *);
    DECLARE_3DSCALAROP(v3fdivi, Vec3Df, int, /);
    DECLARE_3DSCALAROP(v3faddi, Vec3Df, int, +);
    DECLARE_3DSCALAROP(v3fsubi, Vec3Df, int, -);
    DECLARE_3DSCALAROP(v3fshr,  Vec3Df, int, >>);
    DECLARE_3DSCALAROP(v3fshl,  Vec3Df, int, <<);

    DECLARE_3DSCALAROP(v3fmulf, Vec3Df, float, *);
    DECLARE_3DSCALAROP(v3fdivf, Vec3Df, float, /);
    DECLARE_3DSCALAROP(v3faddf, Vec3Df, float, +);
    DECLARE_3DSCALAROP(v3fsubf, Vec3Df, float, -);
#endif

#endif /* VECTOR_H */
