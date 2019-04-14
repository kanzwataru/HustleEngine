#ifndef THREEDEE_H
#define THREEDEE_H

typedef byte   color_t;

typedef struct Transform Transform;
struct Transform {
    Transform *parent;
    Vec3D      position;
    Vec3D      rotation;
    Vec3D      scale;
};

typedef struct {
    Vec3D      vertices[3];
    Vec3D      normal;
    color_t    color;
} Triangle;

typedef struct {
    Transform  xform;
    Triangle   far *tris;
    size_t     tri_count;
} Mesh;

typedef struct {
    Mesh      *meshes;
    size_t     mesh_count;
} Scene;

void draw_tris(buffer_t *buf, Triangle far *tris, size_t count);
void draw_tris_wire(buffer_t *buf, Triangle far *tris, size_t count, color_t color);
void draw_mesh(buffer_t *buf, Mesh *mesh);

#endif
