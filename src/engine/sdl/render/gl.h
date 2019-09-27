#ifndef GL_H
#define GL_H

#include "common/platform.h"
#include "glad/glad.h"

#define VERT_SIZE   3 + 2   /* position, uv */

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
//void gl_cleanup_model(Model *model);
void gl_draw_model(Model *model);

#endif
