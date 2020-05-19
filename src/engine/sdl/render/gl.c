#include "gl.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
static void APIENTRY debug_gl_callback(
    GLenum source,
    GLenum type,
    GLuint id, 
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *user_param
)
{
    (void)source; (void)type; (void)id;
    (void)severity; (void)length; (void)user_param;

    FILE *outf = severity == GL_DEBUG_SEVERITY_LOW ? stdout : stderr;
    fprintf(outf, "%s\n", message);
    if(severity == GL_DEBUG_SEVERITY_HIGH) {
        fprintf(stderr, "*** OpenGL error severity is high, aborting ***");
        abort();
    }   
}
#endif

void gl_init(void)
{
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_gl_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, 1); 
#endif
}

void gl_upload_model(struct Model *model, const float *verts)
{
    glGenVertexArrays(1, &model->vao);
    glGenBuffers(1, &model->vbo);

    glBindVertexArray(model->vao);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, model->vert_count * (5 * sizeof(float)), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void gl_upload_model_instanced(struct Model *model, const float *verts, size_t instance_data_count)
{
    /*
     * Instanced data is two slots of vec4 per model
     */
    glGenVertexArrays(1, &model->vao);
    glGenBuffers(1, &model->vbo);
    glGenBuffers(1, &model->vbo_inst);

    glBindVertexArray(model->vao);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, model->vert_count * (5 * sizeof(float)), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo_inst);
    glBufferData(GL_ARRAY_BUFFER, instance_data_count * sizeof(float) * 8, NULL, GL_STREAM_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(4 * sizeof(float)));
    glVertexAttribDivisor(3, 1);
    
    glBindVertexArray(0);
    
    model->instance_data_count = instance_data_count;
}

void gl_update_instance_buffer(struct Model *model, const float *data, size_t offset, size_t count)
{
    assert(model->vbo_inst);
    assert(offset + count <= model->instance_data_count);
    
    glBindVertexArray(model->vao);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo_inst);
    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(float) * 8, count * sizeof(float) * 8, data);
    
    glBindVertexArray(0);
}

void gl_draw_model(struct Model *model)
{
    glBindVertexArray(model->vao);
    glDrawArrays(GL_TRIANGLES, 0, model->vert_count);
}

void gl_draw_model_instanced(struct Model *model, size_t count)
{
    assert(model->vbo_inst);

    glBindVertexArray(model->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, model->vert_count, count);
}

void gl_delete_model(struct Model *model)
{
    glDeleteVertexArrays(1, &model->vao);
    glDeleteBuffers(1, &model->vbo);
    if(model->vbo_inst) {
        glDeleteBuffers(1, &model->vbo_inst);
    }
}

struct Framebuffer gl_create_framebuffer(Rect size)
{
    struct Framebuffer fb = {size, 0, 0, {0, size}};

    glGenFramebuffers(1, &fb.id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb.id);

    glGenTextures(1, &fb.col_buf.id);
    glBindTexture(GL_TEXTURE_2D, fb.col_buf.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.w, size.h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.col_buf.id, 0);

    glGenRenderbuffers(1, &fb.depth_buf);
    glBindRenderbuffer(GL_RENDERBUFFER, fb.depth_buf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.w, size.h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb.depth_buf);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer not complete!\n");
        assert(0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fb;
}

struct Framebuffer gl_get_backbuffer(Rect size)
{
    struct Framebuffer back_buf = {{0, 0, size.w, size.h}, 0, 0, {0}};

    return back_buf;
}

void gl_set_framebuffer(struct Framebuffer *fb)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
    glViewport(fb->size.x, fb->size.y, fb->size.w, fb->size.h);
}

void gl_delete_framebuffer(struct Framebuffer *fb)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fb->id);
}
