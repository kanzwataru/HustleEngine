#ifndef VIDEO_H
#define VIDEO_H

#include "common/platform.h"

enum VIDEO_MODES {
    VIDEO_MODE_LOW256,
    VIDEO_MODE_HIGH16,
    VIDEO_MODE_HIGH16HALF
};

void video_wait_vsync(void);
void video_init_mode(byte mode, byte scaling);
void video_exit(void);
void video_flip(const buffer_t *backbuf);
void video_set_palette(const buffer_t *palette);
void video_set_color_at(byte id, byte red, byte green, byte blue);

#endif
