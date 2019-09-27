#include "engine/render.h"
#include "platform/sdl/nativeplatform.h"
#include "gl.h"
#include "shaders.h"

struct Texture {

};

struct Framebuffer {

};

static struct PlatformData *platform;
static struct RenderData *rd;

static const float quad[] = {
    0, 1, 0,    0, 1,
    1, 0, 0,    1, 0,
    0, 0, 0,    0, 0,

    0, 1, 0,    0, 1,
    1, 1, 0,    1, 1,
    1, 0, 0,    1, 0
};

/*
static const float identity_matrix[] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

static const float screen_matrix[] = {
    1.0 / (float)WIDTH, 0, 0, 0,
    0, 1.0  (float)HEIGHT, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};
*/

void renderer_init(PlatformData *pd)
{
    renderer_reloaded(pd);

    rd->quad.vert_count = 6;
    rd->flat_shader = gl_compile_shader(planar_vert_src, flat_frag_src);

    glViewport(0, 0, WIDTH, HEIGHT);
    gl_upload_model(&rd->quad, quad);
    
    /* palette texture */
    glGenTextures(1, &rd->palette_tex);
    glBindTexture(GL_TEXTURE_1D, rd->palette_tex);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, PALETTE_COLORS, 0, GL_RGB, GL_UNSIGNED_BYTE, rd->palette);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void renderer_reloaded(PlatformData *pd)
{
    platform = pd;
    rd = &platform->renderer;

    gladLoadGLLoader(SDL_GL_GetProcAddress);
}

void renderer_quit(PlatformData *pd)
{
    glDeleteTextures(1, &rd->palette_tex);
}

void renderer_clear(byte clear_col)
{
    glClearColor((float)clear_col / 255.0f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_flip(void)
{
    SDL_GL_SwapWindow(platform->window_handle);
}

void renderer_set_palette(buffer_t *pal, byte offset, byte count)
{
    memcpy(rd->palette + (offset * 3), pal + (offset * 3), count * 3);
    
    glBindTexture(GL_TEXTURE_1D, rd->palette_tex);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, PALETTE_COLORS, 0, GL_RGB, GL_UNSIGNED_BYTE, rd->palette);
}

void renderer_get_palette(buffer_t *pal, byte offset, byte count)
{
    memcpy(pal + (offset * 3), rd->palette + (offset * 3), count * 3);
}

void renderer_draw_rect(Framebuffer *buf, Rect xform, byte color)
{
    glUseProgram(rd->flat_shader);

    float model_matrix[] = {
        xform.w, 0, 0, 0,
        0, xform.h, 0, 0,
        0, 0, 1, 0,
        xform.x, xform.y, 0, 1
    };

    GLint model_loc = glGetUniformLocation(rd->flat_shader, "model");
    GLint color_loc = glGetUniformLocation(rd->flat_shader, "color");
    GLint screen_size_loc = glGetUniformLocation(rd->flat_shader, "screen_size");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);
    glUniform3f(color_loc, (float)color / 255.0f, 0.5f, 0.5f);
    glUniform2f(screen_size_loc, WIDTH, HEIGHT);

    gl_draw_model(&rd->quad);
}
