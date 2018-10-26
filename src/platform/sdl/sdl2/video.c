/*
 * SDL2 backend Video module
*/
#include "../internal/video_common.h"

static SDL_Window   *window;
static SDL_Renderer *renderer;
static SDL_Texture  *framebuffer;
static SDL_Rect      original_resolution;

static uint32_t     *converted; /* a temp buffer for converting from indexed to rgba */
static SDL_PixelFormat *pixel_format;

static void pixel_format_get(void) 
{
    uint32_t format_id;
    
    SDL_QueryTexture(framebuffer, &format_id, NULL, NULL, NULL);
    pixel_format = SDL_AllocFormat(format_id);
}

void video_init_mode(byte mode, byte scaling)
{
    printf("HustleEngine: SDL2 backend initialization\n");
    
    switch(mode) {
        case VIDEO_MODE_LOW256:
            original_resolution.w = LOW256_WIDTH;
            original_resolution.h = LOW256_HEIGHT;
            break;
        case VIDEO_MODE_HIGH16:
            original_resolution.w = HIGH16_WIDTH;
            original_resolution.h = HIGH16_HEIGHT;
            break;
        case VIDEO_MODE_HIGH16HALF:
            original_resolution.w = HIGH16HALF_WIDTH;
            original_resolution.h = HIGH16HALF_HEIGHT;
            break;
        default:
            PANIC("Invalid video mode");
            break;
    }
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL2 Initialization Failed \n\n%s\n", SDL_GetError());
        exit(1);
    }
    
    window = SDL_CreateWindow("HustleEngine", SDL_WINDOWPOS_UNDEFINED, 
                                              SDL_WINDOWPOS_UNDEFINED,
                                              0, 0,
                                              SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(!window) {
        fprintf(stderr, "Failed to create SDL_Window: %s\n", SDL_GetError());
        exit(1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        fprintf(stderr, "Failed to create SDL_Renderer %s\n", SDL_GetError());
    }
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(renderer, original_resolution.w, original_resolution.h);

    framebuffer = SDL_CreateTexture(renderer, 
                                    SDL_PIXELFORMAT_ARGB8888, 
                                    SDL_TEXTUREACCESS_STREAMING,
                                    original_resolution.w,
                                    original_resolution.h);
    if(!framebuffer) {
        fprintf(stderr, "Failed to create SDL_Texture of size (%d x %d)\n", original_resolution.w, original_resolution.h);
    }

    converted = malloc((original_resolution.w * original_resolution.h) * sizeof(uint32_t));
    if(!converted) {
        fprintf(stderr, "Failed to allocate buffer for Indexed to RGBA conversion\n");
    }
    
    pixel_format_get();
}

void video_exit(void) 
{
    free(converted);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void video_wait_vsync(void) {}

void video_flip(buffer_t *backbuf)
{
    int i;
    uint32_t *dst = converted;
        
    for(i = 0; i < original_resolution.w * original_resolution.h; ++i) {
        *dst++ = SDL_MapRGB(pixel_format, fbpalette[backbuf[i]].r, fbpalette[backbuf[i]].g, fbpalette[backbuf[i]].b);
    }
    
    SDL_UpdateTexture(framebuffer, NULL, converted, original_resolution.w * sizeof(uint32_t));
    SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void video_set_palette(buffer_t *palette)
{
    internal_set_palette(palette);
}
