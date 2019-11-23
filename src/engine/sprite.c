#include "sprite.h"
#include "render.h"
#include "common/mathlib.h"

#define get_frame(_spritesheet, _frame) \
    ((buffer_t *)((buffer_t *)_spritesheet->offset_table + _spritesheet->base_offset + _spritesheet->offset_table[_frame]))

void sprite_update(struct Sprite *spr, size_t count)
{
    size_t i;
    struct SpritesheetAsset *sheet;

    for(i = 0; i < count; ++i) {
        sheet = asset_from_handle(spr[i].spritesheet);
        if(spr[i].frameskip-- < 0) {
            if(++spr[i].current_frame == sheet->count) {
                spr[i].current_frame = 0;
            }
            spr[i].frameskip = sheet->frameskip;
        }
    }
}

void sprite_draw(struct Sprite *spr, size_t count)
{
    size_t i;
    struct SpritesheetAsset *sheet;
    buffer_t *buf;

    for(i = 0; i < count; ++i) {
        sheet = asset_from_handle(spr->spritesheet);
        buf = get_frame(sheet, spr->current_frame);
        renderer_draw_texture(buf, spr->rect);
    }
}
