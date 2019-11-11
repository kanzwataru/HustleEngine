#pragma once
#include "asset_macros.h"

/* types */
struct Texture {
    const char *name;
    const char *path;
    int width, height;
};
config_declare(Texture);

struct Palette {
    const char *name;
    const char *path;
};
config_declare(Palette);

struct Package {
    const char *name;
    const char **contents;
};
config_declare(Package);

#define do_all(_func) \
    _func(Texture); \
    _func(Palette);
