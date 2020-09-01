#include "render_display.h"

static SDL_Renderer *sdl_renderer;
static SDL_Texture *sdl_texture;

void display_init(struct PlatformData *pd)
{
    sdl_renderer = SDL_CreateRenderer(pd->window_handle, SDL_RENDERER_ACCELERATED, SDL_RENDERER_PRESENTVSYNC);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_RenderSetLogicalSize(sdl_renderer, WIDTH, HEIGHT);
    sdl_texture = SDL_CreateTexture(sdl_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
}

void display_quit(struct PlatformData *pd)
{
    SDL_DestroyTexture(sdl_texture);
    SDL_DestroyRenderer(sdl_renderer);
}

void display_present(struct PlatformData *pd)
{
    SDL_UpdateTexture(sdl_texture, NULL, pd->renderer.rgba_buf, 320 * sizeof(uint32_t));
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
}

void display_swap_buffers(struct PlatformData *pd)
{
    SDL_RenderPresent(sdl_renderer);
}
