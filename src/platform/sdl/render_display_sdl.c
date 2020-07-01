#include "render_display.h"

void display_init(struct PlatformData *pd)
{
    pd->sdl_renderer = SDL_CreateRenderer(pd->window_handle, SDL_RENDERER_ACCELERATED, SDL_RENDERER_PRESENTVSYNC);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_RenderSetLogicalSize(pd->sdl_renderer, WIDTH, HEIGHT);
    pd->sdl_texture = SDL_CreateTexture(pd->sdl_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
}

void display_quit(struct PlatformData *pd)
{
    SDL_DestroyTexture(pd->sdl_texture);
    SDL_DestroyRenderer(pd->sdl_renderer);
}

void display_present(struct PlatformData *pd)
{
    SDL_UpdateTexture(pd->sdl_texture, NULL, pd->renderer.rgba_buf, 320 * sizeof(uint32_t));
    SDL_RenderClear(pd->sdl_renderer);
    SDL_RenderCopy(pd->sdl_renderer, pd->sdl_texture, NULL, NULL);
    SDL_RenderPresent(pd->sdl_renderer);
}
