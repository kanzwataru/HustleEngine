#pragma once
#include <stdint.h>
struct ImageInfo {
    int width;
    int height;
};

uint8_t *load_bmp_image(const char *file, struct ImageInfo *out_info);
uint8_t *load_bmp_palette(const char *file);
