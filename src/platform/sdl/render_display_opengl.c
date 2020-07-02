#include "render_display.h"
#include "gl/gl.h"

static const char *shader_present_vert_src =
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"layout (location = 1) in vec2 in_uv;\n"

"uniform vec2 screen_size;"
"uniform vec2 texture_size;"

"out vec2 uv;\n"

"void main() {\n"
"   vec2 pos = vec2(in_pos.x * 2 - 1, (1 - in_pos.y) * 2 - 1);"
"   float screen_aspect = screen_size.x / screen_size.y;"
"   float aspect = texture_size.x / texture_size.y;"
"   float correction = aspect / screen_aspect;"
"   vec2 pos_letterbox = vec2(pos.x * correction, pos.y);"
"   gl_Position = vec4(pos_letterbox, in_pos.z, 1.0);\n"
"   uv = vec2(in_uv.x, in_uv.y);\n"
"}\n\0";

static const char *shader_present_frag_src =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 frag_col;\n"

"uniform sampler2D pixel_target;\n"

"void main() {\n"
"   frag_col = texture(pixel_target, uv);\n"
"}\n\0";

static const float quad[] = {
    0, 1, 0,    0, 1,
    1, 1, 0,    1, 1,
    0, 0, 0,    0, 0,

    1, 1, 0,    1, 1,
    1, 0, 0,    1, 0,
    0, 0, 0,    0, 0
};

static struct Model quad_model = { .vert_count = 6 };
static GLuint shader;
static GLuint texture;

void display_init(struct PlatformData *pd)
{
    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    pd->gl_context = SDL_GL_CreateContext(pd->window_handle);
    if(!pd->gl_context) {
        fprintf(stderr, SDL_GetError());
        exit(1);
    }

    SDL_GL_SetSwapInterval(1);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    gl_upload_model(&quad_model, quad);
    shader = gl_compile_shader(shader_present_vert_src, shader_present_frag_src);

    /* backbuf texture */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "screen_size"), pd->screen_size.w, pd->screen_size.h);
    glUniform2f(glGetUniformLocation(shader, "texture_size"), WIDTH, HEIGHT);
}

void display_quit(struct PlatformData *pd)
{
    gl_delete_model(&quad_model);

    if(pd->gl_context) {
        SDL_GL_DeleteContext(pd->gl_context);
    }
}

void display_present(struct PlatformData *pd)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, WIDTH, HEIGHT,
                    GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                    pd->renderer.rgba_buf);

    gl_draw_model(&quad_model);
}

void display_swap_buffers(struct PlatformData *pd)
{
    SDL_GL_SwapWindow(pd->window_handle);
}
