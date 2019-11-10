/*
    *** HUSTLE ENGINE ***

    Serializable types, used with asset system.

    All structs are headers, with data arrays that
    extend past the end of the struct and into serialized
    data.

    Data fields are opaque and can be platform-specific.
*/
#ifndef ASSET_STRUCTS_H
#include "common/platform.h"

struct TilesetAsset {
    uint16_t width;
    uint16_t height;
    uint16_t flags;
    uint16_t size;
    byte data[1];   /* extends past struct */
};

struct TilemapAsset {
    uint16_t dimensions;
    uint16_t flags;
    uint16_t count;
    uint16_t size;
    byte data[1];   /* extends past struct */
};

struct TextureAsset {
    uint16_t flags;
    uint16_t size;
    byte data[1];   /* extends past struct */
};

#endif
