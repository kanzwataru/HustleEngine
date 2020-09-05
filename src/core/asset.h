#ifndef ASSET_H
#define ASSET_H

#include "asset_structs.h"

/* handle */
struct AssetHandle {
    uint32_t offset;
    byte *pak_file;
};

typedef struct AssetHandle assetid_t;

#define asset_handle_to(_name, _pak, _paktype) \
    asset_make_handle(offsetof(_paktype, _name), (byte *)_pak)

#define asset_from_handle(_handle) \
    ((void *)(_handle.pak_file + _handle.offset))

#define asset_from_handle_of(_handle, _type) \
    ((struct _type##Asset*)(asset_from_handle(_handle)))

static inline struct AssetHandle asset_make_handle(uint32_t offset, void *pak_file)
{
    struct AssetHandle handle;
    handle.offset = offset;
    handle.pak_file = pak_file;

    return handle;
}

static inline bool asset_is_empty(assetid_t asset)
{
    return !asset.offset;
}

/* loading */
void asset_load_pak(byte *asset_pak, size_t max_size, const char *file_name);

#endif
