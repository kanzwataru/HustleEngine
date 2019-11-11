#ifndef ASSET_H
#define ASSET_H

#include "asset_structs.h"

/* convinience macros
 * usage example:
    struct TilemapAsset *map = asset_get(LEVEL_2, Tilemap, pak_file);
    struct TextureAsset *tex = asset_get(SKY, Texture, pak_file);
*/
#define asset_get(_name, _type, _pak) \
    ((struct _type##Asset*)(_pak + ASSET_##_name))

/* asset loading functions */
void asset_load_pak(byte *asset_pak, const char *file_name);

#endif
