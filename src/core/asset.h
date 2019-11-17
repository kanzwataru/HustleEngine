#ifndef ASSET_H
#define ASSET_H

#include "asset_structs.h"

/* convinience macros */
#define asset_get(_name, _type, _pak) \
    ((struct _type##Asset*)(_pak + ASSET_##_name))

#define asset_get_direct(_offset, _type, _pak) \
    ((struct _type##Asset*)(_pak + _offset))

#define asset_sprite_get_frame(_spritesheet, _frame) \
    ((buffer_t *)((buffer_t *)_spritesheet->offset_table + _spritesheet->base_offset + _spritesheet->offset_table[_frame]))

/* asset loading functions */
void asset_load_pak(byte *asset_pak, const char *file_name);

#endif
