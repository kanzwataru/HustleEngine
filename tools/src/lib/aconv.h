/*
 * Asset conversion
 *
 * (split off from HustleEngine, but still using its internal data structures)
*/

#ifndef ASSET_CONVERSION_H
#define ASSET_CONVERSION_H

#include "common/platform.h"
#include "engine/render/rle.h"

struct ImageInfo {
    int width;
    int height;
};

buffer_t *load_bmp_image(const char *file, struct ImageInfo *out_info);
buffer_t *load_bmp_palette(const char *file);

size_t buffer_to_rle(RLEImage *rle, buffer_t *buf, int width, int height);

#endif
