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

struct Tileset {
    uint16 width;
    uint16 height;
    uint16 flags;
    uint16 size;
    byte data[1];   /* extends past struct */
};

struct Tilemap {
    uint16 dimensions;
    uint16 flags;
    uint16 count;
    uint16 size;
    byte data[1];   /* extends past struct */
};

struct Texture {
    uint16 flags;
    uint16 size;
    byte data[1];   /* extends past struct */
};

#endif
