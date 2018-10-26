#ifndef FILESYS_H
#define FILESYS_H

#include "common/platform.h"

buffer_t *load_bmp_image(const char *file);
buffer_t *load_bmp_palette(const char *file);

#endif
