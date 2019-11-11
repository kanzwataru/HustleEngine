#include <cstdlib>
#include <cstdio>
#include <string>
#include "convert.hpp"
extern "C" {
    #include "asset_config.h"
    #include "lib/conv.h"
}

template <typename T>
static const T *find_in(const char *name, const T *config_type, size_t count) {
    if(count == 0) {
        fprintf(stderr, "Type of %s was not found in config\n", name);
        return nullptr;
    }

    for(size_t i = 0; i < count; ++i) {
        if(std::string(config_type[i].name) == std::string(name))
            return &config_type[i];
    }

    fprintf(stderr, "%s not found in config\n", name);
    return nullptr;
}

int texture_convert(const char *name)
{
    const auto *tex = find_in(name, config_get(Texture), config_count(Texture));
    if(!tex)
        return 1;

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
    fwrite(bmp, sizeof(bmp), size, stdout);

    return 0;
}
