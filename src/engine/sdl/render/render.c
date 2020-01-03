#include "internal.h"
#include "engine/init.h"
#include "engine/render.h"
#include "engine/profiling.h"
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

    if(rd->initialized) {
        glDeleteProgram(rd->flat_shader);
        glDeleteProgram(rd->sprite_shader);
        glDeleteProgram(rd->post_shader);
        glDeleteProgram(rd->text_shader);
        glDeleteProgram(rd->tilemap_shader);
        renderer_clear_cache();
    }
    rd->flat_shader = gl_compile_shader(planar_vert_src, flat_frag_src);
    rd->sprite_shader = gl_compile_shader(planar_vert_src, sprite_frag_src);
    rd->post_shader = gl_compile_shader(post_vert_src, post_palette_frag_src);
    rd->text_shader = gl_compile_shader(planar_vert_src, text_frag_src);
    rd->tilemap_shader = gl_compile_shader(planar_vert_src, tilemap_frag_src);

    if(!rd->initialized) {
        rd->quad.vert_count = 6;

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
    PROFILE_SECTION_BEGIN(RENDER_flip)

    gl_set_framebuffer(&rd->back_buf);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(rd->post_shader);
    glBindTexture(GL_TEXTURE_2D, rd->target_buf.col_buf.id);

    gl_draw_model(&rd->quad);

    SDL_GL_SwapWindow(platform->window_handle);
    glClear(GL_COLOR_BUFFER_BIT); /* add a clear request here to force GL flush and block for VSync */
    gl_set_framebuffer(&rd->target_buf);
    
    PROFILE_SECTION_END(RENDER_flip)
    PROFILE_FRAME_END()
    PROFILE_FRAME_BEGIN()
}

void renderer_set_palette(const buffer_t *pal, byte offset, byte count)
{
    PROFILE_SECTION_BEGIN(RENDER_set_palette)
    
    memcpy(rd->palette + (offset * 3), pal + (offset * 3), count * 3);

    glBindTexture(GL_TEXTURE_1D, rd->palette_tex);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, PALETTE_COLORS, 0, GL_RGB, GL_UNSIGNED_BYTE, rd->palette);

    PROFILE_SECTION_END(RENDER_set_palette)
}

void renderer_get_palette(buffer_t *pal, byte offset, byte count)
{
    PROFILE_SECTION_BEGIN(RENDER_get_palette)

    memcpy(pal + (offset * 3), rd->palette + (offset * 3), count * 3);

    PROFILE_SECTION_END(RENDER_get_palette)
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
    PROFILE_SECTION_BEGIN(RENDER_draw_rect)

    glUseProgram(rd->flat_shader);

    GLint color_loc = glGetUniformLocation(rd->flat_shader, "color_id");
    glUniform1f(color_loc, (float)color / 255.0f);

    draw_quad(rd->flat_shader, xform);

    PROFILE_SECTION_END(RENDER_draw_rect)
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
    PROFILE_SECTION_BEGIN(RENDER_local_draw_texture)

    glUseProgram(rd->sprite_shader);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLint sprite_tex_loc = glGetUniformLocation(rd->sprite_shader, "sprite");
    glUniform1i(sprite_tex_loc, 1);
    draw_quad(rd->sprite_shader, xform);

    glBindTexture(GL_TEXTURE_2D, 0); /* TODO: do we need these teardowns? */
    glActiveTexture(GL_TEXTURE0);

    PROFILE_SECTION_END(RENDER_local_draw_texture)
}

void renderer_draw_texture(const struct TextureAsset *texture, Rect xform)
{
    PROFILE_SECTION_BEGIN(RENDER_draw_texture)

    assert(texture->id < CACHE_MAX);

    int id = texture->id;

    if(!rd->cached_textures[id].cached) {
        rd->cached_textures[id].tex = upload_texture(texture->data, texture->width, texture->height);
        rd->cached_textures[id].cached = true;
    }

    draw_texture(rd->cached_textures[id].tex, xform);

    PROFILE_SECTION_END(RENDER_draw_texture)
}

void renderer_draw_sprite(const struct SpritesheetAsset *sheet, const buffer_t *frame, Rect xform)
{
    PROFILE_SECTION_BEGIN(RENDER_draw_sprite)

    GLuint tex = upload_texture(frame, sheet->width, sheet->height);
    draw_texture(tex, xform);
    glDeleteTextures(1, &tex);

    PROFILE_SECTION_END(RENDER_draw_sprite)
}

static int pixel_to_tile(int pixel, int size, int max)
{
    int a = (pixel / size);
    if(a % size) a -= 1;
    if(a < 0) a = 0;
    if(a >= max) a = max - 1;

    return a;
}

void renderer_draw_tilemap(const struct TilemapAsset *map, const struct TilesetAsset *tiles, Point offset)
{
    PROFILE_SECTION_BEGIN(RENDER_draw_tilemap)
    int ty, tx;
    Rect rect;
    uint16_t *ids = (uint16_t *)map->data;

    assert(tiles->tile_size == map->tile_size);
    rect.w = tiles->tile_size;
    rect.h = tiles->tile_size;

    const int max_width = 2 + pixel_to_tile(offset.x + platform->target_size.w, tiles->tile_size, map->width);
    const int max_height = 2 + pixel_to_tile(offset.y + platform->target_size.h, tiles->tile_size, map->height);

    /* set up shader and uniforms */
    if(!rd->cached_textures[tiles->id].cached) {
        rd->cached_textures[tiles->id].tex = upload_texture(tiles->data, tiles->width, tiles->height);
        rd->cached_textures[tiles->id].cached = true;
    }

    glUseProgram(rd->tilemap_shader);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, rd->cached_textures[tiles->id].tex);
    const GLint sprite_tex_loc = glGetUniformLocation(rd->tilemap_shader, "sprite");
    glUniform1i(sprite_tex_loc, 1);
    const GLint uv_rect_loc = glGetUniformLocation(rd->tilemap_shader, "uv_rect");

    for(ty = pixel_to_tile(offset.y, tiles->tile_size, map->height); ty < max_height; ++ty) {
        for(tx = pixel_to_tile(offset.x, tiles->tile_size, map->width); tx < max_width; ++tx) {
            rect.x = (tiles->tile_size * tx) - offset.x;
            rect.y = (tiles->tile_size * ty) - offset.y;

            int tile_id = ids[ty * map->width + tx];

            const float uv_rect[4] = {
                tile_id,
                (tile_id * tiles->tile_size) / tiles->height,
                (float)tiles->tile_size / tiles->width,   // this is how much to scale DOWN the uv
                (float)tiles->tile_size / tiles->height
            };
            glUniform4fv(uv_rect_loc, 1, uv_rect);
            
            draw_quad(rd->tilemap_shader, rect);

            /*
            // draw wireframe of quads
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glUseProgram(rd->flat_shader);
            //draw_quad(rd->tilemap_shader, rect);
            GLint color_loc = glGetUniformLocation(rd->flat_shader, "color_id");
            glUniform1f(color_loc, 2 / 255.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUseProgram(rd->tilemap_shader);
            */
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0); /* TODO: do we need these teardowns? */
    glActiveTexture(GL_TEXTURE0);

    PROFILE_SECTION_END(RENDER_draw_tilemap)
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
    PROFILE_SECTION_BEGIN(RENDER_draw_line)

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

    PROFILE_SECTION_END(RENDER_draw_line)
}


void renderer_draw_text(const struct FontAsset *font, const char *str, byte color, Rect bounds)
{
    PROFILE_SECTION_BEGIN(RENDER_draw_text)

    /* cache font if needed */
    assert(font->id < CACHE_MAX);
    
    const int id = font->id;
    
    if(!rd->cached_textures[id].cached) {
        rd->cached_textures[id].tex = upload_texture(font->data, font->width, font->height);
        rd->cached_textures[id].cached = true;
    }

    const GLuint tex = rd->cached_textures[id].tex;

    /* set up shader and uniforms */
    glUseProgram(rd->text_shader);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, tex);

    const float b = (float)rd->palette[(color * 3) + 0] / 255.0f;
    const float g = (float)rd->palette[(color * 3) + 1] / 255.0f;
    const float r = (float)rd->palette[(color * 3) + 2] / 255.0f;
    const GLint color_loc = glGetUniformLocation(rd->text_shader, "color");
    glUniform3f(color_loc, r, g, b);

    const GLint sprite_tex_loc = glGetUniformLocation(rd->text_shader, "sprite");
    glUniform1i(sprite_tex_loc, 1);

    const GLint uv_rect_loc = glGetUniformLocation(rd->text_shader, "uv_rect");

    /* do text drawing */
    const char *c = str;
    int x = 0;
    int y = 0;
    while(*c) {
        switch(*c) {
        case ' ':
            ++c;
            x += font->font_size;
            break;
        case '\t':
            ++c;
            x += font->font_size *4;
            break;
        case '\n':
            y += font->font_size;
            x = 0;
            ++c;
            continue;
        default:
            if(*c < 33)
                ++c;
            break;
        }

        if(x > bounds.w - font->font_size) {
            y += font->font_size;
            x = 0;
        }

        const Rect xform = {bounds.x + x, bounds.y + y, font->font_size, font->font_size};
        
        const int char_offset = *c - 33; // shift everything starting from ASCII '!'
        const float uv_rect[] = {
            char_offset, // NOTE: this works because of wrap-around, won't work for DOS
            (char_offset * font->font_size) / font->height,
            (float)font->font_size / font->width,   // this is how much to scale DOWN the uv
            (float)font->font_size / font->height
        };

        if(xform.x + xform.w > bounds.w + bounds.x ||
           xform.y + xform.h > bounds.y + bounds.h)
        {
            break;
        }

        glUniform4fv(uv_rect_loc, 1, uv_rect);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //glUseProgram(rd->flat_shader);
        draw_quad(rd->text_shader, xform);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ++c;
        x += font->font_size;
    }

    glBindTexture(GL_TEXTURE_2D, 0); /* TODO: do we need these teardowns? */
    glActiveTexture(GL_TEXTURE0);

    PROFILE_SECTION_END(RENDER_draw_text)
}
