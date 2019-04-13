/*
 * Barebones linear algebra math library
*/
#ifndef MATH3D_H
#define MATH3D_H

#include "common/prim.h"
#include "common/math.h"
#include <math.h>

/*
 * Scalar math
*/
#ifndef M_PI
    #define M_PI 3.1415f
#endif

#define RADIANS(X) ((X * M_PI) / 180)

float fast_invsqrt(float value);

/*
 * Vector math
*/
#define VEC_ADD(V, A, B)         (V).x = (A).x + (B).x; (V).y = (A).y + (B).y; (V).z = (A).z + (B).z;
#define VEC_SUB(V, A, B)         (V).x = (A).x - (B).x; (V).y = (A).y - (B).y; (V).z = (A).z - (B).z;
#define VEC_MUL(V, A, B)         (V).x = (A).x * (B).x; (V).y = (A).y * (B).y; (V).z = (A).z * (B).z;
#define VEC_DIV(V, A, B)         (V).x = (A).x / (B).x; (V).y = (A).y / (B).y; (V).z = (A).z / (B).z;

#define VEC_ADD_SCALAR(V, A, B)  (V).x = (A).x + (B); (V).y = (A).y + (B); (V).z = (A).z + (B);
#define VEC_SUB_SCALAR(V, A, B)  (V).x = (A).x - (B); (V).y = (A).y - (B); (V).z = (A).z - (B);
#define VEC_MUL_SCALAR(V, A, B)  (V).x = (A).x * (B); (V).y = (A).y * (B); (V).z = (A).z * (B);
#define VEC_DIV_SCALAR(V, A, B)  (V).x = (A).x / (B); (V).y = (A).y / (B); (V).z = (A).z / (B);

#define VEC4_ADD(V, A, B)        V[0] = A[0] + B[0]; V[1] = A[1] + B[1]; V[2] = A[2] + B[2]; V[3] = A[3] + B[3];
#define VEC4_SUB(V, A, B)        V[0] = A[0] - B[0]; V[1] = A[1] - B[1]; V[2] = A[2] - B[2]; V[3] = A[3] - B[3];
#define VEC4_MUL(V, A, B)        V[0] = A[0] * B[0]; V[1] = A[1] * B[1]; V[2] = A[2] * B[2]; V[3] = A[3] * B[3];
#define VEC4_DIV(V, A, B)        V[0] = A[0] / B[0]; V[1] = A[1] / B[1]; V[2] = A[2] / B[2]; V[3] = A[3] / B[3];

#define VEC4_ADD_SCALAR(V, A, B) V[0] = A[0] + B; V[1] = A[1] + B; V[2] = A[2] + B; V[3] = A[3] + B;
#define VEC4_SUB_SCALAR(V, A, B) V[0] = A[0] - B; V[1] = A[1] - B; V[2] = A[2] - B; V[3] = A[3] - B;
#define VEC4_MUL_SCALAR(V, A, B) V[0] = A[0] * B; V[1] = A[1] * B; V[2] = A[2] * B; V[3] = A[3] * B;
#define VEC4_DIV_SCALAR(V, A, B) V[0] = A[0] / B; V[1] = A[1] / B; V[2] = A[2] / B; V[3] = A[3] / B;

void vec_normalize(Vec3D *vec);
void vec4_normalize(Vec4D vec);

/*
 * Matrix math
*/
#define MAT_IDENTITY {{1, 0, 0, 0}, \
                      {0, 1, 0, 0}, \
                      {0, 0, 1, 0}, \
                      {0, 0, 0, 1}}

void mat_mul_vec(Vec4D vec, Matrix mat);
void mat_mul_mat(Matrix out, Matrix a, Matrix b);
void mat_translate(Matrix mat, Vec3D vec);
void mat_rotate(Matrix mat, float angle, Vec3D axis);
void mat_scale(Matrix mat, Vec3D vec);
void mat_perspective_make(Matrix mat, float fovy, float aspect_ratio, float near_plane, float far_plane);

#endif /* VECTOR_H */
