#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <limits>
#include <memory>
#include "convert.hpp"
#include "util.hpp"
extern "C" {
    #include "asset_config.h"
    #include "lib/conv.h"
}

std::string global::platform;

#define STB_TRUETYPE_IMPLEMENTATION
#include "extern/stb_truetype.h"

#define require_fit(_type, _x) require_fit_impl<_type>(name, stringify_a(_x), _x)

template <typename T>
static T require_fit_impl(const char *asset_name, const char *val_name, long long man)
{
    if(man > std::numeric_limits<T>::max() || man < std::numeric_limits<T>::min()) {
        fprintf(stderr, "asset: %s value: %s does not fit in the required range: %d %d\n",
                asset_name, val_name, std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

        exit(1);
    }

    return T(man);
}

static void write_as_strip(const uint8_t *buf, int width, int height, int tile_size)
{
    for(int ty = 0; ty < height / tile_size; ++ty) {
        for(int tx = 0; tx < width / tile_size; ++tx) {
            const uint8_t *line = buf + ((ty * tile_size) * width + (tx * tile_size));
            assert(line <= (buf + (width * height)));

            for(int y = 0; y < tile_size; ++y) {
                fwrite(line, 1, tile_size, stdout);
                line += width;
            }
        }
    }
}

int texture_convert(const char *name, uint16_t id)
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
    uint16_t header[5] = {id, uint16_t(tex->width), uint16_t(tex->height), 0, size};
    fwrite(&header, sizeof(header), 1, stdout);
    fwrite(bmp, 1, size, stdout);

    return 0;
}

int palette_convert(const char *name, uint16_t id)
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

int spritesheet_convert(const char *name, uint16_t id)
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

    uint16_t id_out = id;
    uint8_t header[4] = {
        require_fit(uint8_t, spr->width),
        require_fit(uint8_t, spr->height),
        require_fit(uint8_t, spr->count),
        require_fit(uint8_t, spr->frameskip)
    };

    uint16_t flags = 0;
    if(spr->play_once)
        flags |= 0x01;

    uint16_t base_offset = spr->count * sizeof(uint16_t);

    std::vector<uint16_t> offset_table;
    for(int i = 0; i < spr->count; ++i) {
        offset_table.push_back(i * (spr->width * spr->height));
    }

    /* write out spritesheet */
    fwrite(&id_out, sizeof(id_out), 1, stdout);
    fwrite(header, sizeof(header), 1, stdout);
    fwrite(&flags, sizeof(flags), 1, stdout);
    fwrite(&base_offset, sizeof(base_offset), 1, stdout);
    fwrite(&offset_table[0], sizeof(uint16_t), offset_table.size(), stdout);

    fwrite(bmp, 1, image_info.width * image_info.height, stdout);

    return 0;
}

int tileset_convert(const char *name, uint16_t id)
{
    const auto *set = config_find_in(name, config_get(Tileset), config_count(Tileset));
    if(!set) {
        fprintf(stderr, "%s not found in config\n", name);
        return 1;
    }

    ImageInfo image_info;
    uint8_t *bmp = load_bmp_image(set->path, &image_info);
    if(image_info.width != set->width || image_info.height != set->height) {
        fprintf(stderr, "\t%s: Dimension mismatch\n", name);
        return 1;
    }

    int tile_count = (set->width / set->tile_size) * (set->height / set->tile_size);

    /* write out in strip */
    uint16_t header[4] = {
        require_fit(uint16_t, id),
        require_fit(uint16_t, set->tile_size),
        require_fit(uint16_t, set->tile_size),
        require_fit(uint16_t, tile_count),
    };

    fwrite(&header, sizeof(header), 1, stdout);
    write_as_strip(bmp, image_info.width, image_info.height, set->tile_size);

    return 0;
}

int tilemap_convert(const char *name, uint16_t id)
{
    const auto *map = config_find_in(name, config_get(Tilemap), config_count(Tilemap));
    if(!map) {
        fprintf(stderr, "%s not found in config\n", name);
        return 1;
    }

    size_t map_size = map->width * map->height;
    size_t size = map_size * sizeof(uint16_t);
    std::unique_ptr<uint8_t> map_data(new uint8_t[size]);

    /* read in */
    FILE *fp = fopen(map->path, "rb");
    fread(map_data.get(), 1, size, fp);

    /* write out */
    uint16_t header[5] = {
        require_fit(uint16_t, id),
        require_fit(uint16_t, map->width),
        require_fit(uint16_t, map->height),
        require_fit(uint16_t, map->tile_size),
        0
    };
    fwrite(header, sizeof(header), 1, stdout);
    fwrite(map_data.get(), 1, size, stdout);
    fclose(fp);

    return 0;
}

int font_convert(const char *name, uint16_t id)
{
    const auto *font = config_find_in(name, config_get(Font), config_count(Font));
    if(!font) {
        fprintf(stderr, "%s not found in config\n", name);
        return 1;
    }

    /* image buffer */
    const int font_size = 8; /* TODO: hard-coded for now */
    const int image_width = 128;
    const int image_height = 128;
    const int per_line = image_width / font_size;
    
    std::unique_ptr<uint8_t> image_buf(new uint8_t[image_width * image_height]);
    memset(image_buf.get(), 0, image_width * image_height);

    /* load font */
    size_t size;
    FILE *font_file = fopen(font->path, "rb");
    fseek(font_file, 0, SEEK_END);
    size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    std::unique_ptr<uint8_t> font_buf(new uint8_t[size]);
    
    fread(font_buf.get(), size, 1, font_file);
    fclose(font_file);

    stbtt_fontinfo info;
    if(!stbtt_InitFont(&info, font_buf.get(), 0)) {
        fprintf(stderr, "Could not load font %s from %s (stbtt_InitFont failed)\n", font->name, font->path);
        return 1;
    }

    const float scale = stbtt_ScaleForPixelHeight(&info, font_size);

    int x = 0;
    int line = 0;

    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);

    ascent *= scale;
    descent *= scale;

    /* render ASCII table to bitmap */
    int char_count = 0;
    for(char c = 33; c < 127; ++c) {
        int min_x, min_y, max_x, max_y;
        stbtt_GetCodepointBitmapBox(&info, c, scale, scale, &min_x, &min_y, &max_x, &max_y);

        const int y = (line * font_size) + (ascent + min_y);
        stbtt_MakeCodepointBitmap(&info, image_buf.get() + (x + (y * image_width)), max_x - min_x, max_y - min_y, image_width, scale, scale, c);
    
        x += font_size; /* assume square character, fixed width */
        if(++char_count == per_line) {
            char_count = 0;
            ++line;
            x = 0;
        }
    }

    for(int i = 0; i < image_width * image_height; ++i) {
        /* use only 0 and 1 */
        image_buf.get()[i] = image_buf.get()[i] == 0 ? 0 : 1;
    }

    /* write out asset */
    uint8_t header[2] = {
        require_fit(uint8_t, font_size),
        0
    };

    fwrite(header, sizeof(header), 1, stdout);

    if(global::platform == "unix") {
        /* write out directly as atlas texture (to use with opengl) */
        fwrite(image_buf.get(), 1, image_width * image_height, stdout);
    }
    else if(global::platform == "dos") {
        /* write out in one long strip (like spritesheets) */
        write_as_strip(image_buf.get(), image_width, image_height, font_size);
    }
    else {
        assert(0);
    }

    return 0;
}
