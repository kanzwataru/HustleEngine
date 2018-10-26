#include "platform/filesys.h"
#include "common/platform.h"
#include <stdio.h>
#include <stdlib.h>

struct FileLoadData {
    FILE *fp;
    uint16 col_num, width, height;
};

static void fskip(FILE *fp, int num_bytes)
{
    int i;
    for(i = 0; i < num_bytes; ++i)
        fgetc(fp);
}

static struct FileLoadData open_bmp_file(const char *file)
{
    struct FileLoadData d;
    size_t ret;

    /* open the file */
    if ((d.fp = fopen(file,"rb")) == NULL) {
        while(1) printf("Error opening file %s\n", file);
    }

    /* read in the width and height of the image, and the
    number of colors used; ignore the rest */
    fskip(d.fp,18);
    ret = fread(&d.width, sizeof(uint16), 1, d.fp);  assert(ret == 1);
    fskip(d.fp,2);
    ret = fread(&d.height,sizeof(uint16), 1, d.fp);  assert(ret == 1);
    fskip(d.fp,22);
    ret = fread(&d.col_num,sizeof(uint16), 1, d.fp); assert(ret == 1);
    fskip(d.fp,6);

    /* assume we are working with an 8-bit file */
    if (d.col_num == 0) d.col_num = 256;

    return d;
}

/* 
 * http://www.brackeen.com/vga/source/djgpp20/palette.c.html
*/
buffer_t *load_bmp_image(const char *file)
{
    long i;
    int x;
    struct FileLoadData d = open_bmp_file(file);

    buffer_t *buf = farcalloc(d.width * d.height, sizeof(byte));
    if(!buf)
        while(1) printf("Out of mem: load_bmp_image %s\n", file);

    /* ignore palette */
    fskip(d.fp,d.col_num * 4);

    for(i = (d.height - 1) * d.width; i >= 0; i -= d.width)
        for(x = 0; x < d.width; ++x)
            buf[(size_t)(i + x)] = (byte)fgetc(d.fp);
    
    fclose(d.fp);

    return buf;
}

buffer_t *load_bmp_palette(const char *file)
{
    int i;
    struct FileLoadData d = open_bmp_file(file);

    buffer_t *palette = farcalloc(256 * 3, sizeof(buffer_t));
    if(!palette)
        while(1) printf("Out of mem: load_bmp_palette %s\n", file);

    for(i = 0; i < d.col_num * 3; i+= 3) {
        palette[i + 2] = fgetc(d.fp) >> 2;
        palette[i + 1] = fgetc(d.fp) >> 2;
        palette[i + 0] = fgetc(d.fp) >> 2;
        fgetc(d.fp);
    
        //printf("%s [%d] (%d %d %d)\n", file, i, palette[i + 0], palette[i + 1], palette[i + 2]);
    }

    fclose(d.fp);

    return palette;
}
