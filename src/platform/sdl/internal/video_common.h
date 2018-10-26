#ifndef VIDEO_SDL_COMMON_H
#define VIDEO_SDL_COMMON_H

#include "sdl_shim.h"
#include "common/platform.h"
#include "platform/video.h"

#define LOW256_WIDTH        320
#define LOW256_HEIGHT       200

#define HIGH16_WIDTH        640
#define HIGH16_HEIGHT       480

#define HIGH16HALF_WIDTH    640
#define HIGH16HALF_HEIGHT   200

#define PALETTE_SIZE        256

static byte video_mode;
static SDL_Color fbpalette[PALETTE_SIZE];

static void internal_set_palette(buffer_t *palette)
{
    printf("palette set\n");
    switch(video_mode) {
        case VIDEO_MODE_LOW256:
            for(int i = 0; i < 256; ++i) {
                /* left-shift to convert the VGA-compatible
                 * 6-bit colours back to 8-bit colours */
                fbpalette[i].r = *palette++ << 2;
                fbpalette[i].g = *palette++ << 2;
                fbpalette[i].b = *palette++ << 2;
            }
            break;
        case VIDEO_MODE_HIGH16:
        case VIDEO_MODE_HIGH16HALF:
            for(int i = 0; i < 16; ++i) {
                /* left-shift to convert the VGA-compatible
                 * 6-bit colours back to 8-bit colours */
                fbpalette[i].r = *palette++ << 2;
                fbpalette[i].g = *palette++ << 2;
                fbpalette[i].b = *palette++ << 2;
            }
            break;
        default:
            PANIC("Invalid video mode");
            break;
    }
}

#endif
