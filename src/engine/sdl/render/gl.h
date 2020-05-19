#ifndef GL_H
#define GL_H

#include "common/platform.h"
#include "common/mathlib.h"
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
struct Model {
    //float *verts;
    uint32_t vert_count;
    uint32_t instance_data_count;
    GLuint vao;
    GLuint vbo;
    GLuint vbo_inst;
};

void gl_init(void);

shaderid_t gl_compile_shader(const char *vert_src, const char *frag_src);

void gl_upload_model(struct Model *model, const float *verts);
void gl_upload_model_instanced(struct Model *model, const float *verts, size_t instance_data_count);
void gl_update_instance_buffer(struct Model *model, const float *data, size_t offset, size_t count);
void gl_delete_model(struct Model *model);
void gl_draw_model(struct Model *model);
void gl_draw_model_instanced(struct Model *model, size_t count);

struct Framebuffer gl_create_framebuffer(Rect size);
struct Framebuffer gl_get_backbuffer(Rect size);
void gl_set_framebuffer(struct Framebuffer *fb);
void gl_delete_framebuffer(struct Framebuffer *fb);

#endif
