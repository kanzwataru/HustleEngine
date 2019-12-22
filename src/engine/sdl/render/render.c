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
    renderer_clear_cache();
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

void renderer_draw_rect(byte color, Rect xform)
{
    glUseProgram(rd->flat_shader);

    GLint color_loc = glGetUniformLocation(rd->flat_shader, "color");
    glUniform3f(color_loc, (float)color / 255.0f, 0.0f, 0.0f);

    draw_quad(rd->flat_shader, xform);
}

static GLuint upload_texture(const buffer_t *buf, int width, int height)
{
    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buf);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return tex;
}

static void draw_texture(GLuint tex, Rect xform)
{
    glUseProgram(rd->sprite_shader);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLint sprite_tex_loc = glGetUniformLocation(rd->sprite_shader, "sprite");
    glUniform1i(sprite_tex_loc, 1);
    draw_quad(rd->sprite_shader, xform);

    glBindTexture(GL_TEXTURE_2D, 0); /* TODO: do we need these teardowns? */
    glActiveTexture(GL_TEXTURE0);
}

void renderer_draw_texture(const struct TextureAsset *texture, Rect xform)
{
    assert(texture->id < CACHE_MAX);

    int id = texture->id;

    if(!rd->cached_textures[id].cached) {
        rd->cached_textures[id].tex = upload_texture(texture->data, texture->width, texture->height);
        rd->cached_textures[id].cached = true;
    }

    draw_texture(rd->cached_textures[id].tex, xform);
}

void renderer_draw_sprite(const struct SpritesheetAsset *sheet, const buffer_t *frame, Rect xform)
{
    GLuint tex = upload_texture(frame, sheet->width, sheet->height);
    draw_texture(tex, xform);
    glDeleteTextures(1, &tex);
}

void renderer_draw_tilemap(const struct TilemapAsset *map, const struct TilesetAsset *tiles, Point offset)
{
    int ty, tx;
    Rect rect;
    uint16_t *ids = (uint16_t *)map->data;

    assert(tiles->tile_size == map->tile_size);
    rect.w = tiles->tile_size;
    rect.h = tiles->tile_size;

    for(ty = 0; ty < map->height; ++ty) {
        for(tx = 0; tx < map->width; ++tx) {
            rect.x = (tiles->tile_size * tx) - offset.x;
            rect.y = (tiles->tile_size * ty) - offset.y;


            int tile_id = ids[ty * map->width + tx];
            const buffer_t *buf = &tiles->data[(tiles->tile_size * tiles->tile_size) * tile_id];

            // TODO: don't be hacky
            int cache_id = 2000 + (1000 * tiles->id) + tile_id;
            assert(cache_id < CACHE_MAX);
            if(!rd->cached_textures[cache_id].cached) {
                rd->cached_textures[cache_id].tex = upload_texture(buf, rect.w, rect.h);
                rd->cached_textures[cache_id].cached = true;
            }

            draw_texture(rd->cached_textures[cache_id].tex, rect);
        }
    }
}

void renderer_clear_cache(void)
{
    for(int i = 0; i < CACHE_MAX; ++i) {
        if(rd->cached_textures[i].cached) {
            glDeleteTextures(1, &rd->cached_textures[i].tex);
            rd->cached_textures[i].cached = false;
        }
    }
}

void renderer_draw_line(byte color, const Point *line, size_t count)
{
    assert(count > 1);
    struct Model model;

    /* TODO: don't use dynamic allocation */
    float *line_verts = calloc(count * 5, sizeof(float)); 
    for(size_t i = 0; i < count; ++i) {
        line_verts[0 + (i * 5)] = line[i].x;
        line_verts[1 + (i * 5)] = line[i].y;
    }

    model.vert_count = count;
    gl_upload_model(&model, line_verts);

    glUseProgram(rd->flat_shader);

    GLint color_loc = glGetUniformLocation(rd->flat_shader, "color");
    glUniform3f(color_loc, (float)color / 255.0f, 0.0f, 0.0f);

    const float model_matrix[] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    GLint model_loc = glGetUniformLocation(rd->flat_shader, "model");
    GLint screen_size_loc = glGetUniformLocation(rd->flat_shader, "screen_size");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);
    glUniform2f(screen_size_loc, WIDTH, HEIGHT);

    glBindVertexArray(model.vao);
    glDrawArrays(GL_LINES, 0, model.vert_count);

    gl_delete_model(&model);
    free(line_verts);
}

