#include "render_display.h"
#include "gl/gl.h"
#include <stdlib.h>

static struct Model quad_model = { .vert_count = 6 };
static GLuint shader;
static GLuint texture;

static char *file_load(const char *path)
{
    FILE *fp = fopen(path, "r");
    if(!fp)
        return NULL;

    fseek(fp, 0, SEEK_END);
    const size_t size = ftell(fp);
    rewind(fp);

    char *data = malloc(size + 1);
    assert(data);

    size_t read_bytes = fread(data, 1, size, fp);
    assert(read_bytes == size);

    data[size] = '\0';

    fclose(fp);
    return data;
}

static shaderid_t load_compile_shader(const char *vert, const char *frag)
{
    char *vert_src = file_load(vert);
    char *frag_src = file_load(frag);
    assert(vert_src && frag_src);

    shaderid_t shader = gl_compile_shader(vert_src, frag_src);

    free(vert_src);
    free(frag_src);

    return shader;
}

void display_init(struct PlatformData *pd)
{
    const float texture_aspect = (float)pd->target_size.w / (float)pd->target_size.h;
    const float screen_aspect = (float)pd->screen_size.w / (float)pd->screen_size.h;
    const float aspect_correct = texture_aspect / screen_aspect;

    int width = pd->screen_size.w;
    int height = pd->screen_size.h;
    if(aspect_correct < 1.0f) {
        width = floor((float)width * aspect_correct);
    }
    else {
        // TODO: 怪しい
        height = floor((float)height * aspect_correct);
    }

    const int xmin = (pd->screen_size.w - width) / 2;
    const int xmax = xmin + width;
    const int ymin = (pd->screen_size.h - height) / 2;
    const int ymax = ymin + height;

    const float xminf = (float)xmin / (float)pd->screen_size.w;
    const float xmaxf = (float)xmax / (float)pd->screen_size.w;
    const float yminf = (float)ymin / (float)pd->screen_size.h;
    const float ymaxf = (float)ymax / (float)pd->screen_size.h;

    const float quad[] = {
        xminf, ymaxf, 0,    0, 1,
        xmaxf, ymaxf, 0,    1, 1,
        xminf, yminf, 0,    0, 0,

        xmaxf, ymaxf, 0,    1, 1,
        xmaxf, yminf, 0,    1, 0,
        xminf, yminf, 0,    0, 0
    };

    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    pd->gl_context = SDL_GL_CreateContext(pd->window_handle);
    if(!pd->gl_context) {
        fputs(SDL_GetError(), stderr);
        exit(1);
    }

    SDL_GL_SetSwapInterval(1);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    gl_upload_model(&quad_model, quad);
    shader = load_compile_shader("shaders/present_simple_vert.glsl", "shaders/present_simple_frag.glsl");

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
