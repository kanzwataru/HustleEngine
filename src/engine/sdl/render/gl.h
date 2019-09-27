#ifndef GL_H
#define GL_H

#include "common/platform.h"
#include "common/structures.h"
#include "glad/glad.h"

#define VERT_SIZE               3 + 2   /* position, uv */

struct Texture {
    GLuint  id;
    Rect    size;
};

struct Framebuffer {
    Rect    size;
    GLuint  id;
    GLuint  depth_buf;
    struct Texture col_buf;
};

typedef GLuint shaderid_t;
typedef GLuint textureid_t;
typedef struct Model {
    //float *verts;
    uint32 vert_count;
    GLuint vao;
    GLuint vbo;
} Model;

shaderid_t gl_compile_shader(const char *vert_src, const char *frag_src);

void gl_upload_model(Model *model, const float *verts);
void gl_delete_model(Model *model);
void gl_draw_model(Model *model);

struct Framebuffer gl_create_framebuffer(Rect size);
void gl_set_framebuffer(struct Framebuffer *fb);
void gl_delete_framebuffer(struct Framebuffer *fb);

#endif
