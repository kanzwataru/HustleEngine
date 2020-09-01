// TODO: remove repetition with macros
// TODO: eventually have reflection?
#pragma once

#define HE_PLATFORM_SDL2 // TODO: this is a hack
#include "core/asset_structs.h"
#include "asset_config.h"

enum AssetType {
    ASSET_Tileset,
    ASSET_Tilemap,
    ASSET_Texture,
    ASSET_Palette,
    ASSET_Font,
    ASSET_Spritesheet,

    ASSET_Total
};

static const char *asset_name_table[ASSET_Total] = {
    "Tileset",
    "Tilemap",
    "Texture",
    "Palette",
    "Font",
    "Spritesheet"
};

static size_t asset_size_table[ASSET_Total] = {
    sizeof(struct TilesetAsset),
    sizeof(struct TilemapAsset),
    sizeof(struct TextureAsset),
    sizeof(struct PaletteAsset),
    sizeof(struct FontAsset),
    sizeof(struct SpritesheetAsset)
};
