/*
 * SDL1.2 backend Video module
*/
#include "../internal/video_common.h"

#include "common/math.h"

static SDL_Surface *screen;   /* screen surface (scaled) */
static SDL_Surface *framebuffer; /* based on render.c buffer, at original resolution*/
static SDL_Rect     native_resolution;
static SDL_Rect     original_resolution;
static SDL_Rect     logical_screen;
static int scale;

static int to_prev_pow2(int n) {
    /* convert to previous power of two */
    int p2 = 1;
    while(p2 <= n)
        p2 <<= 1;

    return p2 >> 1;
}

static SDL_Rect get_native_resolution(void)
{
    SDL_Rect **modes;
    int i;
    
    modes = SDL_ListModes(NULL, SDL_FULLSCREEN);
    if(modes == NULL)
        PANIC("No modes");
    
    if(modes == (SDL_Rect **)-1) {
        PANIC("All resolutions available. What does that mean?");
    }
    else {
        printf("Available Modes: \n\n");
        for(i = 0; modes[i]; ++i)
            printf("->  %d x %d\n", modes[i]->w, modes[i]->h);
    }
    
    return *(modes[0]);
}

static SDL_Rect fit_rect(SDL_Rect large, SDL_Rect small, int *scale_out)
{
    float scale = MIN(large.w / small.w, large.h / small.h);
    int proper_scale = to_prev_pow2((int)scale);
    
    SDL_Rect out = {0, 0, small.w * proper_scale, small.h * proper_scale};    
    *scale_out = proper_scale;
    
    if((float)large.w / (float)large.h >= 2) {
        /* if the screen is super wide we probably have dual monitors
         * so just make sure to be on the right-most one (dirty hack)
        */
        out.x = large.w - out.w;
        out.y = (large.h - out.h) / 2;
    }
    else {
        /* center on screen */
        out.x = (large.w - out.w) / 2;
        out.y = (large.h - out.h) / 2;
    }
    
    return out;
}

/*
 * Video API init
*/
void video_init_mode(byte mode, byte scaling)
{
    assert(scaling > 0);
    video_mode = mode;
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
        fprintf(stderr, "SDL Initialization Failed \n\n%s\n", SDL_GetError());
        exit(1);
    }
    
    native_resolution = get_native_resolution();
    logical_screen    = fit_rect(native_resolution, original_resolution, &scale);
    
    /* use the native resolution and then we can handle the scaling ourselves */
    screen = SDL_SetVideoMode(native_resolution.w, native_resolution.h, 24, SDL_SWSURFACE | SDL_ANYFORMAT | SDL_FULLSCREEN | SDL_DOUBLEBUF);
    if(!screen) {
        fprintf(stderr, "Failed to set video mode (%d x %d)\n%s\n", original_resolution.w * scale, original_resolution.h * scale, SDL_GetError());
        exit(1);
    }
    
    /* framebuffer colour mode is 8-bit indexed (256 col) */
    framebuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, logical_screen.w, logical_screen.h, 8, 0,0,0,0);
    if(!framebuffer) {
        fprintf(stderr, "Failed to create framebuffer (%d x %d)\n%s\n", original_resolution.w, original_resolution.h, SDL_GetError());
        exit(1);
    }
}

/*
 * Video API exit
*/
void video_exit(void)
{
    SDL_FreeSurface(framebuffer);
    SDL_Quit();
}

/*
 * Block and wait for vsync
*/
void video_wait_vsync(void)
{
    /* vsync should be provided by SDL
     * in SDL_Flip and we should be able to do the
     * framebuffer transfer during Vblank easily on modern hardware
     */
}

/*
 * Video API flip buffer
 *
 * We have to do a bit o' finangling to get the VGA mode 13h stuff to draw on an SDL surface
*/
void video_flip(buffer_t *backbuf)
{
    int doubling = 0, lines = 0, pixels = 0;
    
    byte *linehead = framebuffer->pixels;
    byte *src = backbuf;
    
    /* we ONLY support Mode 13h for now!! */
    if(video_mode != VIDEO_MODE_LOW256)
        NOT_IMPLEMENTED;
    
    if(SDL_MUSTLOCK(framebuffer)) SDL_LockSurface(framebuffer);
    
    /* copy original to scaled framebuffer, doubling up lines where necessary */
    while(lines < logical_screen.h) {
        pixels = 0;
        doubling = scale;
        while(pixels < logical_screen.w) {
            while(doubling --> 0) {
                *(linehead + pixels) = *src;
                ++pixels;
            }
            
            doubling = scale;
            ++src;
        }
        
        ++lines;
        
        doubling = scale - 1;
        while(doubling --> 0) {
            memcpy(linehead + logical_screen.w, linehead, logical_screen.w);
            linehead += logical_screen.w;
            ++lines;
        }
        
        linehead += logical_screen.w;
    }
    if(SDL_MUSTLOCK(framebuffer)) SDL_UnlockSurface(framebuffer);
    
    SDL_BlitSurface(framebuffer, NULL, screen, &logical_screen);
    SDL_Flip(screen);
}

/*
 * Sets colour palette
 *
 * Depending on the video mode it may be 16 or 256 colours
*/
void video_set_palette(buffer_t *palette)
{
    internal_set_palette(palette);

    if(0 == SDL_SetPalette(framebuffer, SDL_LOGPAL | SDL_PHYSPAL, fbpalette, 0, 256)) {
        printf("%s\n", SDL_GetError());
        PANIC("Set palette failed");
    }
}
