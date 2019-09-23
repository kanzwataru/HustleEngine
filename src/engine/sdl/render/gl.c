#include "gl.h"
#include "gl_debug.h"

void gl_upload_model(Model *model, const float *verts)
{
    glGenVertexArrays(1, &model->vao);
    glGenBuffers(1, &model->vbo);

    glBindVertexArray(model->vao);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, model->vert_count * (5 * sizeof(float)), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERT_SIZE * sizeof(float), (void *)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    //glBindVertexArray(0);

    check_gl_error();
}

void gl_draw_model(Model *model)
{
    glBindVertexArray(model->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    check_gl_error();
}
