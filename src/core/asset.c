#include "asset.h"
#include <stdio.h>

void asset_load_pak(byte *asset_pak, const char *file_name)
{
    uint32_t size = 0;
    FILE *fp = fopen(file_name, "rb");
    assert(fp);

    fread(&size, sizeof(uint32_t), 1, fp);
    assert(size);

    fread(asset_pak, 1, size, fp);

    fclose(fp);
}
