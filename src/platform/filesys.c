#include "platform/filesys.h"

void *load_asset_pak(const char *file)
{
    FILE   *fp;
    byte   *data;
    uint32  size;

    fp = fopen(file, "rb");
    assert(fp);

    fread(&size, sizeof(uint32), 1, fp);
    DEBUG_DO(printf("* loading asset pak: %s (%d bytes)\n", file, size));
    data = malloc(size);
    assert(data);

    fread(data, sizeof(byte), size, fp);

    fclose(fp);

    return data;
}
