#include <cstdlib>
#include <cstdio>
#include <string>
#include "convert.hpp"
#include "util.hpp"
extern "C" {
    #include "asset_config.h"
    #include "lib/conv.h"
}

int texture_convert(const char *name)
{
    const auto *tex = config_find_in(name, config_get(Texture), config_count(Texture));
    if(!tex) {
        fprintf(stderr, "%s not found in config\n", name);
        return 1;
    }

    ImageInfo image_info;
    uint8_t *bmp = load_bmp_image(tex->path, &image_info);
    if(image_info.width != tex->width || image_info.height != tex->height) {
        fprintf(stderr, "\t%s: Dimension mismatch", name);
        return 1;
    }

    /* write out flat bitmap */
    uint16_t size = tex->width * tex->height;
    uint16_t header[4] = {uint16_t(tex->width), uint16_t(tex->height), 0, size};
    fwrite(&header, sizeof(header), 1, stdout);
    fwrite(bmp, 1, size, stdout);

    return 0;
}

int palette_convert(const char *name)
{
    const auto *pal = config_find_in(name, config_get(Palette), config_count(Palette));
    if(!pal) {
        fprintf(stderr, "%s not found in config\n", name);
        return 1;
    }

    uint8_t *pal_data = load_bmp_palette(pal->path);    /* will always give a 256-col buffer */

    /* write out palette */
    uint8_t size = 255;     /* assume 256-colour palette */
    fwrite(&size, sizeof(size), 1, stdout);
    fwrite(pal_data, 1, (size + 1) * 3, stdout);

    return 0;
}
