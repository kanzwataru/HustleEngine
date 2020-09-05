#include "asset.h"
#include <stdio.h>

void asset_load_pak(byte *asset_pak, size_t max_size, const char *file_name)
{
    uint32_t size = 0;
    FILE *fp = fopen(file_name, "rb");
    assert(fp);

    fread(&size, sizeof(uint32_t), 1, fp);
    assert(size);
    assert(size <= max_size);

    fread(asset_pak, 1, size, fp);

    fclose(fp);

#if 0
    byte *p = asset_pak;
    for(int i = 0; i < size / 8; ++i) {
        for(int j = 0; j < 8; ++j) {
            printf("%02X ", *p++);
        }
        printf("\n");
    }
#endif
}
