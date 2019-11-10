#include "internal.h"
#include "engine/init.h"
#include "engine/render.h"
#include "platform/sdl/nativeplatform.h"
#include "gl.h"
#include "shaders.h"

static struct PlatformData *platform;
static struct RenderData *rd;

static const float quad[] = {
    0, 1, 0,    0, 1,
    1, 1, 0,    1, 1,
    0, 0, 0,    0, 0,

    1, 1, 0,    1, 1,
    1, 0, 0,    1, 0,
    0, 0, 0,    0, 0
};

void renderer_init(struct PlatformData *pd)
{
    platform = pd;
    rd = &platform->renderer;

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    if(!rd->initialized) {
        rd->quad.vert_count = 6;
        rd->flat_shader = gl_compile_shader(planar_vert_src, flat_frag_src);
        rd->sprite_shader = gl_compile_shader(planar_vert_src, sprite_frag_src);
        rd->post_shader = gl_compile_shader(post_vert_src, post_palette_frag_src);
        rd->target_buf = gl_create_framebuffer(platform->target_size);
        rd->back_buf = gl_get_backbuffer(platform->screen_size);

        gl_set_framebuffer(&rd->target_buf);
        gl_upload_model(&rd->quad, quad);

        /* palette texture */
        glGenTextures(1, &rd->palette_tex);
        glBindTexture(GL_TEXTURE_1D, rd->palette_tex);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, PALETTE_COLORS, 0, GL_RGB, GL_UNSIGNED_BYTE, rd->palette);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        rd->initialized = true;
    }
}

void renderer_quit(struct PlatformData *pd)
{
    glDeleteTextures(1, &rd->palette_tex);
    gl_delete_model(&rd->quad);
    gl_delete_framebuffer(&rd->target_buf);

    rd->initialized = false;
}

void renderer_clear(byte clear_col)
{
//    glClearColor((float)clear_col / 255.0f, 0.5f, 0.5f, 1.0f);
    byte *col = rd->palette + (clear_col * 3);

    glClearColor((float)col[0] / 255, (float)col[1] / 255, (float)col[2] / 255, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void renderer_flip(void)
{
    gl_set_framebuffer(&rd->back_buf);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(rd->post_shader);
    glBindTexture(GL_TEXTURE_2D, rd->target_buf.col_buf.id);

    gl_draw_model(&rd->quad);

    SDL_GL_SwapWindow(platform->window_handle);

    gl_set_framebuffer(&rd->target_buf);
}

void renderer_set_palette(const buffer_t *pal, byte offset, byte count)
{
    memcpy(rd->palette + (offset * 3), pal + (offset * 3), count * 3);

    glBindTexture(GL_TEXTURE_1D, rd->palette_tex);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, PALETTE_COLORS, 0, GL_RGB, GL_UNSIGNED_BYTE, rd->palette);
}

void renderer_get_palette(buffer_t *pal, byte offset, byte count)
{
    memcpy(pal + (offset * 3), rd->palette + (offset * 3), count * 3);
}

static void draw_quad(shaderid_t shader, Rect xform)
{
    float model_matrix[] = {
        xform.w, 0, 0, 0,
        0, xform.h, 0, 0,
        0, 0, 1, 0,
        xform.x, xform.y, 0, 1
    };

    GLint model_loc = glGetUniformLocation(shader, "model");
    GLint screen_size_loc = glGetUniformLocation(shader, "screen_size");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);
    glUniform2f(screen_size_loc, WIDTH, HEIGHT);

    gl_draw_model(&rd->quad);
}

void renderer_draw_rect(Rect xform, byte color)
{
    glUseProgram(rd->flat_shader);

    GLint color_loc = glGetUniformLocation(rd->flat_shader, "color");
    glUniform3f(color_loc, (float)color / 255.0f, 0.0f, 0.0f);

    draw_quad(rd->flat_shader, xform);
}

void renderer_draw_texture(void *texture, Rect xform)
{
    GLuint id;

    glUseProgram(rd->sprite_shader);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, xform.w, xform.h, 0, GL_RED, GL_UNSIGNED_BYTE, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    draw_quad(rd->sprite_shader, xform);
    //renderer_draw_rect(xform, 255);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &id);
}
