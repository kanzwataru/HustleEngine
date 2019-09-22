#include "engine/render.h"
#include "glad/glad.h"
#include "platform/sdl/nativeplatform.h"

struct Texture {

};

struct Framebuffer {

};

static struct PlatformData *platform;
static struct RenderData *rd;

/*
static void check_shader_compilation(GLuint shader)
{
    GLint status;
    char  msg_buf[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if(status != GL_TRUE) {
        glGetShaderInfoLog(shader, 512, NULL, msg_buf);
        fprintf(stderr, "*** SHADER COMPILATION FAILED ***\n, %s\n\n", msg_buf);
    }
}

static void check_program_compilation(GLuint program)
{
    GLint status;
    char  msg_buf[512];

    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if(status != GL_TRUE) {
        glGetProgramInfoLog(program, 512, NULL, msg_buf);
        fprintf(stderr, "*** PROGRAM COMPILATION FAILED ***\n, %s\n\n", msg_buf);
    }
}

#define check_gl_error()    _check_gl_error(__FILE__, __LINE__)
static int _check_gl_error(const char *file, int line)
{
    GLenum err = 0;
    while((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "%s:%d -> OpenGL Error: %04x\n", file, line, err);
    }

    return err == 0;
}
*/

void renderer_init(PlatformData *pd)
{
    renderer_reloaded(pd);
    glViewport(0, 0, 320, 200);
}

void renderer_reloaded(PlatformData *pd)
{
    platform = pd;
    rd = &platform->renderer;

    gladLoadGLLoader(SDL_GL_GetProcAddress);
}

void renderer_quit(PlatformData *pd)
{

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

void renderer_draw_rect(Framebuffer *buf, Rect xform, byte color)
{

}
