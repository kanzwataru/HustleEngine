#include "internal.h"
#include "engine/init.h"
#include "engine/render.h"
#include "platform/sdl/nativeplatform.h"
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
    gl_init();

    if(rd->initialized) {
        glDeleteProgram(rd->flat_shader);
        glDeleteProgram(rd->post_shader);
        renderer_clear_cache();
    }
    rd->flat_shader = gl_compile_shader(planar_vert_src, flat_frag_src);
    rd->post_shader = gl_compile_shader(post_vert_src, post_palette_frag_src);
    rd->flat_shader_inst = gl_compile_shader(planar_vert_inst_src, flat_frag_inst_src);

    if(!rd->initialized) {
        rd->quad.vert_count = 6;
        rd->quad_instanced.vert_count = 6;

        rd->target_buf = gl_create_framebuffer(platform->target_size);
        rd->back_buf = gl_get_backbuffer(platform->screen_size);

        gl_set_framebuffer(&rd->target_buf);
        gl_upload_model(&rd->quad, quad);
        gl_upload_model_instanced(&rd->quad_instanced, quad, MAX_INSTANCE_COUNT);

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
    gl_delete_model(&rd->quad_instanced);
    gl_delete_framebuffer(&rd->target_buf);

    rd->initialized = false;
}

void renderer_clear(byte clear_col)
{
    glClearColor((float)clear_col / 255.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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

    glUseProgram(shader);
    GLint model_loc = glGetUniformLocation(shader, "model");
    GLint screen_size_loc = glGetUniformLocation(shader, "screen_size");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model_matrix);
    glUniform2f(screen_size_loc, WIDTH, HEIGHT);

    gl_draw_model(&rd->quad);
}

static void draw_quad_instanced(shaderid_t shader, struct InstanceBuffer *inst_buf, size_t count)
{
    glUseProgram(shader);
    GLint screen_size_loc = glGetUniformLocation(shader, "screen_size");
    glUniform2f(screen_size_loc, WIDTH, HEIGHT);

    gl_update_instance_buffer(&rd->quad_instanced, inst_buf, 0, count);
    gl_draw_model_instanced(&rd->quad_instanced, count);
}

static void commit_draw_queue(void)
{
    assert(rd->queue.count <= MAX_INSTANCE_COUNT);
    if(rd->queue.count == 0)
        return;
    
    switch(rd->queue.type) {
    case DRAW_RECT: {
        for(int i = 0; i < rd->queue.count; ++i) {
            struct DrawCmd *cmd = &rd->queue.cmds[i];
            struct InstanceBuffer *buf = &rd->queue.instance_buf[i];
            buf->rect[0] = cmd->rect.xform.x;
            buf->rect[1] = cmd->rect.xform.y;
            buf->rect[2] = cmd->rect.xform.w;
            buf->rect[3] = cmd->rect.xform.h;
            buf->data[0] = (float)cmd->rect.color / 255.0f;
        }
        
        draw_quad_instanced(rd->flat_shader_inst, rd->queue.instance_buf, rd->queue.count);
        
        break;
    }
    case DRAW_NULL:
        return;
    default:
        assert(0);
    }
    
    rd->queue.count = 0;
    rd->queue.type = DRAW_NULL;
}

void renderer_draw_rect(byte color, Rect xform)
{
#if 0
    glUseProgram(rd->flat_shader);

    GLint color_loc = glGetUniformLocation(rd->flat_shader, "color_id");
    glUniform1f(color_loc, (float)color / 255.0f);

    draw_quad(rd->flat_shader, xform);
#else
    if(rd->queue.type != DRAW_RECT || rd->queue.count == MAX_INSTANCE_COUNT) {
        commit_draw_queue();
        rd->queue.type = DRAW_RECT;
    }
    
    struct DrawCmd cmd = {
        .rect = {
            .xform = xform,
            .color = color
        }
    };
    
    rd->queue.cmds[rd->queue.count++] = cmd;
#endif
}

#if 0
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
#endif

void renderer_draw_texture(const struct TextureAsset *texture, Rect xform)
{

}

void renderer_draw_sprite(const struct SpritesheetAsset *sheet, const buffer_t *frame, Rect xform)
{

}

void renderer_draw_tilemap(const struct TilemapAsset *map, const struct TilesetAsset *tiles, Point offset)
{

}

void renderer_clear_cache(void)
{

}

void renderer_draw_line(byte color, const Point *line, size_t count)
{

}


void renderer_draw_text(const struct FontAsset *font, const char *str, byte color, Rect bounds)
{

}

void renderer_flip(void)
{
    commit_draw_queue();

    gl_set_framebuffer(&rd->back_buf);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(rd->post_shader);
    GLint pal_loc = glGetUniformLocation(rd->post_shader, "palette");
    GLint pix_loc = glGetUniformLocation(rd->post_shader, "pixel_target");
    glUniform1i(pal_loc, 0);
    glUniform1i(pix_loc, 1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, rd->palette_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, rd->target_buf.col_buf.id);

    gl_draw_model(&rd->quad);

    SDL_GL_SwapWindow(platform->window_handle);

    gl_set_framebuffer(&rd->target_buf);
}
