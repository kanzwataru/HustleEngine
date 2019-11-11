#include <cstdio>
#include "asset/convert.hpp"
#include "asset_config.h"

int main(int argc, char **argv)
{
    printf("Hello from mkasset!\n");

    for(int i = 0; i < config_size(Texture); ++i) {
        printf("Texture\n");
        printf("\tname:%s\n", config_get(Texture)[i].name);
        printf("\tpath:%s\n", config_get(Texture)[i].path);
    }

    for(int i = 0; i < config_size(Palette); ++i) {
        printf("Palette\n");
        printf("\tname:%s\n", config_get(Palette)[i].name);
        printf("\tpath:%s\n", config_get(Palette)[i].path);
    }
}
