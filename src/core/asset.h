#ifndef ASSET_H
#define ASSET_H

#include "asset_structs.h"

/* handle */
struct AssetHandle {
    uint32_t offset;
    byte *pak_file;
};

typedef struct AssetHandle assetid_t;

/* convinience macros */
#define asset_get(_name, _type, _pak) \
    ((struct _type##Asset*)(_pak + ASSET_##_name))

#define asset_get_direct(_offset, _type, _pak) \
    ((struct _type##Asset*)(_pak + _offset))

#define asset_from_handle(_handle) \
    ((void*)(_handle.pak_file + _handle.offset))

#define asset_from_handle_of(_handle, _type) \
    ((struct _type##Asset*)(_handle.pak_file + _handle.offset))

#define asset_handle_to(_name, _type, _pak) \
    asset_make_handle(ASSET_##_name, _pak)

struct AssetHandle asset_make_handle(uint32_t offset, byte *pak_file);

/* asset loading functions */
void asset_load_pak(byte *asset_pak, const char *file_name);

#endif
