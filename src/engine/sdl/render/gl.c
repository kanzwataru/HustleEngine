#include "gl.h"
#include "gl_debug.h"
#include <stdio.h>

void gl_upload_model(Model *model, const float *verts)
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

    //glBindVertexArray(0);

    check_gl_error();
}

void gl_draw_model(Model *model)
{
    glBindVertexArray(model->vao);
    glDrawArrays(GL_TRIANGLES, 0, model->vert_count);
    check_gl_error();
}

void gl_delete_model(Model *model)
{
    glDeleteVertexArrays(1, &model->vao);
    glDeleteBuffers(1, &model->vbo);
    check_gl_error();
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
    check_gl_error();

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
    check_gl_error();
}

void gl_delete_framebuffer(struct Framebuffer *fb)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fb->id);
    check_gl_error();
}
