/*
    *** HUSTLE ENGINE ***

    Serializable types, used with asset system.

    All structs are headers, with data arrays that
    extend past the end of the struct and into serialized
    data.

    Data fields are opaque and can be platform-specific.
*/
#ifndef ASSET_STRUCTS_H
#define ASSET_STRUCTS_H
#include "common/platform.h"

struct TilesetAsset {
    uint16_t id;
    uint16_t tile_size;
    uint16_t count;
    uint16_t flags;
    byte data[1];    /* extends past struct */
};

struct TilemapAsset {
    uint16_t id;
    uint16_t width;
    uint16_t height;
    uint16_t tile_size;
    uint16_t flags;
    byte data[1];    /* extends past struct */
};

struct TextureAsset {
    uint16_t id;
    uint16_t width;  /* width of texture */
    uint16_t height; /* height of texture */
    uint16_t flags;
    uint16_t size;   /* in bytes */
    byte data[1];    /* extends past struct */
};

struct PaletteAsset {
    byte col_count;  /* number of colours - 1 */
    byte data[1];    /* extends past struct */
};


#define SPRITESHEET_FLAG_PLAYONCE   0x01
struct SpritesheetAsset {
    uint16_t id;
    byte width;               /* width of sprites */
    byte height;              /* height of sprites */
    byte count;               /* number of sprites */
    byte frameskip;           /* frames to skip when animating */
    uint16_t flags;
    uint16_t base_offset;     /* offset past offset_table where the first texture can be found */
    uint16_t offset_table[1]; /* extends past struct */
};

#endif

