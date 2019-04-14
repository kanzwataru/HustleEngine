#include "common/platform.h"
#include "common/math3d.h"

float fast_invsqrtf(float value) {
    /* Quake-style */
    long i;
    float x2, y;

    x2 = value * 0.5f;
    y  = value;

    i = *(long *)&y;                      /* begin dark magic */
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (1.5f - (x2 * y * y));
    y = y * (1.5f - (x2 * y * y));

    return y;
}

float remap(float value, float oldmin, float oldmax, float newmin, float newmax)
{
    float real_range = oldmax - oldmin;
    float new_range = newmax - newmin;
    return (((value - oldmin) * new_range) / real_range) + newmin;
}

float vec_dot(Vec3D a, Vec3D b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void vec_normalize(Vec3D *vec) {
    float inv_length = fast_invsqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
    VEC_MUL_SCALAR(*vec, *vec, inv_length);

    /*
    float length = sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
    VEC_DIV_SCALAR(*vec, *vec, length);
    */
}

void vec4_normalize(Vec4D vec) {
    float inv_length = fast_invsqrtf(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);
    VEC4_MUL_SCALAR(vec, vec, inv_length);
}

void mat_mul_vec(Vec4D vec, Matrix mat) {
    Vec4D o;

    o[0] = vec[0] * mat[0][0] + vec[1] * mat[1][0] + vec[2] * mat[2][0] + mat[3][0] * vec[3];
    o[1] = vec[0] * mat[0][1] + vec[1] * mat[1][1] + vec[2] * mat[2][1] + mat[3][1] * vec[3];
    o[2] = vec[0] * mat[0][2] + vec[1] * mat[1][2] + vec[2] * mat[2][2] + mat[3][3] * vec[3];
    o[3] = vec[0] * mat[0][3] + vec[1] * mat[1][3] + vec[2] * mat[2][3] + mat[3][3] * vec[3];

    memcpy(vec, o, sizeof(Vec4D));
}

void mat_mul_vec3(Vec3D *vec, Matrix mat) {
    Vec3D o;
    float w;

    o.x = vec->x * mat[0][0] + vec->y * mat[1][0] + vec->z * mat[2][0] + mat[3][0];
    o.y = vec->x * mat[0][1] + vec->y * mat[1][1] + vec->z * mat[2][1] + mat[3][1];
    o.z = vec->x * mat[0][2] + vec->y * mat[1][2] + vec->z * mat[2][2] + mat[3][3];
    w = vec->x * mat[0][2] + vec->y * mat[1][2] + vec->z * mat[2][2] + mat[3][3];

    if(w != 0.0f) {
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }

    memcpy(vec, &o, sizeof(Vec3D));
}

void mat_mul_mat(Matrix out, Matrix a, Matrix b) {
    int c, r;

    for(c = 0; c < 4; ++c) {
        for(r = 0; r < 4; ++r) {
            out[r][c] = a[r][0] * b[0][c] + a[r][1] * b[1][c] + a[r][2] * b[2][c] + a[r][3] * b[3][c];
        }
    }
}

void mat_translate(Matrix mat, Vec3D vec) {
    Vec4D va, vb, vc;

    VEC4_MUL_SCALAR(va, mat[0], vec.x);
    VEC4_MUL_SCALAR(vb, mat[1], vec.y);
    VEC4_MUL_SCALAR(vc, mat[2], vec.z);

    VEC4_ADD(mat[3], mat[3], va);
    VEC4_ADD(mat[3], mat[3], vb);
    VEC4_ADD(mat[3], mat[3], vc);
}

void mat_rotate(Matrix mat, float angle, Vec3D axis) {
    /* big thanks to CGLM */
    float c;
    Vec3D v, vs;
    Matrix rot = MAT_IDENTITY;
    Matrix out;

    c = cos(angle);
    vec_normalize(&axis);
    VEC_MUL_SCALAR(v, axis, 1.0f - c);
    VEC_MUL_SCALAR(vs, axis, sin(angle));

    VEC4_MUL_SCALAR(rot[0], ((float*)&axis), v.x);
    VEC4_MUL_SCALAR(rot[1], ((float*)&axis), v.y);
    VEC4_MUL_SCALAR(rot[2], ((float*)&axis), v.z);

    rot[0][0] += c;       rot[1][0] -= vs.z;    rot[2][0] += vs.y;
    rot[0][1] += vs.z;    rot[1][1] += c;       rot[2][1] -= vs.x;
    rot[0][2] -= vs.y;    rot[1][2] += vs.x;    rot[2][2] += c;

    rot[0][3] = rot[1][3] = rot[2][3] = rot[3][0] = rot[3][1] = rot[3][2] = 0.0f;
    rot[3][3] = 1.0f;

    mat_mul_mat(out, rot, mat);
    memcpy(mat, out, sizeof(Matrix));
}

void mat_scale(Matrix mat, Vec3D vec) {
    VEC4_MUL_SCALAR(mat[0], mat[0], vec.x);
    VEC4_MUL_SCALAR(mat[1], mat[1], vec.y);
    VEC4_MUL_SCALAR(mat[2], mat[2], vec.z);
}

void mat_perspective_make(Matrix mat, float fovy, float aspect_ratio, float near_plane, float far_plane) {
    float fov = 1.0f / tan(fovy * 0.5f);
    float far_near = 1.0f / (near_plane - far_plane);

    memset(mat, 0, sizeof(Matrix));

    mat[0][0] = fov / aspect_ratio;
    mat[1][1] = fov;
    mat[2][2] = (near_plane + far_plane) * far_near;
    mat[2][3] = -1.0f;
    mat[3][2] = 2.0f * near_plane * far_plane * far_near;
}
