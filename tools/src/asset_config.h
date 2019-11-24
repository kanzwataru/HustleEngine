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

struct Spritesheet {
    const char *name;
    const char *path;
    int width, height;
    int count;
    int frameskip;
};
config_declare(Spritesheet);

struct Tileset {
    const char *name;
    const char *path;
    int width, height;
    int tile_size;
};
config_declare(Tileset);

struct Tilemap {
    const char *name;
    const char *path;
    int width, height;
    int tile_size;
};
config_declare(Tilemap);

struct Package {
    const char *name;
    const char **contents;
};
config_declare(Package);

#define do_all(_func) \
    _func(Texture); \
    _func(Palette); \
    _func(Spritesheet); \
    _func(Tileset); \
    _func(Tilemap);
