#include <cstdio>
#include <map>
#include <string>

#include "asset/convert.hpp"

extern "C" {
    #include "asset_config.h"
}

typedef int (*conversion_handler)(const char *);
std::map<std::string, conversion_handler> converters = {
    {"Texture", texture_convert},
    {"Palette", palette_convert},
    {"Spritesheet", spritesheet_convert},
    {"Tilemap", tilemap_convert},
    {"Tileset", tileset_convert}
};

void help(void)
{
    fprintf(stderr, "mkasset [type] [name] > [out file]\n");
}

int main(int argc, char **argv)
{
    if(argc < 3) {
        help();
        return 1;
    }

    const char *type = argv[1];
    const char *name = argv[2];

    if(converters.count(type)) {
        return converters[type](name);
    }
    else {
        fprintf(stderr, "type: %s is not supported\n", type);
        return 1;
    }
}
