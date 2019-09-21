#include "engine/render.h"
#include "platform/sdl/nativeplatform.h"

struct Texture {
    SDL_Texture *tex;
};

struct Framebuffer {

};

static struct PlatformData *platform;
static struct RenderData *rd;

void renderer_init(PlatformData *pd)
{
    platform = pd;
    rd = &platform->renderer;
}

void renderer_reloaded(PlatformData *pd)
{
    platform = pd;
    rd = &platform->renderer;
}

void renderer_quit(PlatformData *pd)
{

}

void renderer_clear(byte clear_col)
{
    SDL_SetRenderDrawColor(platform->render_handle, clear_col, clear_col, clear_col, 255);
    SDL_RenderClear(platform->render_handle);
}

void renderer_flip(void)
{
    SDL_RenderPresent(platform->render_handle);
}

void renderer_draw_rect(Framebuffer *buf, Rect xform, byte color)
{
    SDL_Rect rect = {xform.x, xform.y, xform.w, xform.h};

    SDL_SetRenderDrawColor(platform->render_handle, color, color, color, 255);
    SDL_RenderFillRect(platform->render_handle, &rect);
}
