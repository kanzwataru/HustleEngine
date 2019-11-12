#include "conv.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

struct FileLoadData {
    FILE *fp;
    uint32_t width, height, col_num, data_offset;
};

static void fskip(FILE *fp, size_t num_bytes)
{
    size_t i;
    for(i = 0; i < num_bytes; ++i)
        fgetc(fp);
}

static struct FileLoadData open_bmp_file(const char *file)
{
    size_t ret;
    struct FileLoadData d;

    /* open the file */
    d.fp = fopen(file, "rb");
    assert(d.fp);

    /* read in the width and height of the image, and the
    number of colors used; ignore the rest */
    fskip(d.fp, 10);
    ret = fread(&d.data_offset, sizeof(uint32_t), 1, d.fp);  assert(ret == 1);
    fskip(d.fp, 4);
    ret = fread(&d.width, sizeof(uint32_t), 1, d.fp);  assert(ret == 1);
    ret = fread(&d.height,sizeof(uint32_t), 1, d.fp);  assert(ret == 1);
    fskip(d.fp, 20);
    ret = fread(&d.col_num,sizeof(uint32_t), 1, d.fp); assert(ret == 1);
    fskip(d.fp, 4);

    /* assume we are working with an 8-bit file */
    if (d.col_num == 0) d.col_num = 256;

    return d;
}

uint8_t *load_bmp_image(const char *file, struct ImageInfo *out_info)
{
    size_t y;
    size_t ret;
    size_t size;
    struct FileLoadData d;
    uint8_t *buf;
    uint8_t *p;

    d = open_bmp_file(file);
    size = d.width * d.height;

    buf = malloc(size);
    assert(buf);

    /* skip directly to data */
    rewind(d.fp);
    fskip(d.fp, d.data_offset);

    /* BMP files are vertically flipped so read in backwards */
    p = buf + (size - d.width);
    for(y = 0; y < d.height; ++y) {
        ret = fread(p, sizeof(uint8_t), d.width, d.fp);
        assert(ret == d.width);
        p -= d.width;
    }

    fclose(d.fp);

    if(out_info) {
        out_info->width = d.width;
        out_info->height = d.height;
    }

    return buf;
}

uint8_t *load_bmp_palette(const char *file)
{
    int i;
    struct FileLoadData d = open_bmp_file(file);

    uint8_t *palette = malloc(256 * 3);
    assert(palette);

    for(i = 0; i < d.col_num * 3; i+= 3) {
        palette[i + 2] = fgetc(d.fp);
        palette[i + 1] = fgetc(d.fp);
        palette[i + 0] = fgetc(d.fp);
        fgetc(d.fp);
    }

    fclose(d.fp);

    return palette;
}
