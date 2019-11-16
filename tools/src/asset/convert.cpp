#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <limits>
#include "convert.hpp"
#include "util.hpp"
extern "C" {
    #include "asset_config.h"
    #include "lib/conv.h"
}

#define require_fit(_type, _x) require_fit_impl<_type>(name, stringify_a(_x), _x)

template <typename T>
T require_fit_impl(const char *asset_name, const char *val_name, long long man)
{
    if(man > std::numeric_limits<T>::max() || man < std::numeric_limits<T>::min()) {
        fprintf(stderr, "asset: %s value: %s does not fit in the required range: %d %d\n",
                asset_name, val_name, std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

        exit(1);
    }

    return T(man);
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
        fprintf(stderr, "\t%s: Dimension mismatch\n", name);
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

int spritesheet_convert(const char *name)
{
    const auto *spr = config_find_in(name, config_get(Spritesheet), config_count(Spritesheet));
    if(!spr) {
        fprintf(stderr, "%s not found in config\n", name);
        return 1;
    }

    ImageInfo image_info;
    uint8_t *bmp = load_bmp_image(spr->path, &image_info);

    /* check dimensions match, taking into account the vertical strip nature of the spritesheet */
    if(image_info.width != spr->width || image_info.height != spr->height * spr->count) {
        fprintf(stderr, "\t%s: Dimension mismatch\n", name);
        return 1;
    }

    uint8_t header[4] = {
        require_fit(uint8_t, spr->width),
        require_fit(uint8_t, spr->height),
        require_fit(uint8_t, spr->count),
        require_fit(uint8_t, spr->frameskip)
    };

    uint16_t flags = 0;
    uint16_t base_offset = spr->count * sizeof(uint16_t);

    std::vector<uint16_t> offset_table;
    for(int i = 0; i < spr->count; ++i) {
        offset_table.push_back(i * (spr->width * spr->height));
    }

    /* write out spritesheet */
    fwrite(header, sizeof(header), 1, stdout);
    fwrite(&flags, sizeof(flags), 1, stdout);
    fwrite(&base_offset, sizeof(base_offset), 1, stdout);
    fwrite(&offset_table[0], offset_table.size(), 1, stdout);

    fwrite(bmp, 1, image_info.width * image_info.height, stdout);

    return 0;
}
