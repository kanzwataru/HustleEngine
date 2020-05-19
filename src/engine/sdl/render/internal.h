#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H
#include "engine/sdl/render/gl.h"

#define MAX_INSTANCE_COUNT  256

enum DrawCmdType {
    DRAW_NULL,
    DRAW_RECT,
    DRAW_SPRITE
};

struct DrawCmd {
    union {
        struct {
            Rect xform;
            byte color;
        } rect;
        struct {
            
        } sprite;
    }
};

struct InstanceBuffer {
    float rect[4];
    float data[4];
};

struct DrawCmdStack {
    enum DrawCmdType type;
    struct InstanceBuffer instance_buf[MAX_INSTANCE_COUNT];
    struct DrawCmd cmds[MAX_INSTANCE_COUNT];
    size_t count;
};

struct RenderData {
    bool     initialized;
    uint16_t flags;

    byte palette[256 * 3];
    textureid_t palette_tex;

    struct Model quad;
    struct Model quad_instanced;
    struct Framebuffer target_buf;
    struct Framebuffer back_buf;
    shaderid_t flat_shader;
    shaderid_t post_shader;
    shaderid_t flat_shader_inst;
    
    struct DrawCmdStack queue;
};

#endif
