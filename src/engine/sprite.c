#include "sprite.h"
#include "render.h"
#include "common/mathlib.h"

#define get_frame(_spritesheet, _frame) \
    ((buffer_t *)((buffer_t *)_spritesheet->offset_table + _spritesheet->base_offset + _spritesheet->offset_table[_frame]))

void sprite_set_to(struct Sprite *spr, assetid_t sheet)
{
    struct SpritesheetAsset *sheet_asset = asset_from_handle(sheet);
    spr->spritesheet = sheet;
    spr->frameskip = sheet_asset->frameskip;
    spr->rect.w = sheet_asset->width;
    spr->rect.h = sheet_asset->height;
}

void sprite_update(struct Sprite *spr, size_t count)
{
    size_t i;
    struct SpritesheetAsset *sheet;

    for(i = 0; i < count; ++i) {
        if(asset_is_empty(spr[i].spritesheet))
            continue;

        sheet = asset_from_handle(spr[i].spritesheet);
        if(spr[i].frameskip-- < 0) {
            if(++spr[i].current_frame == sheet->count) {
                if(sheet->flags & SPRITESHEET_FLAG_PLAYONCE)
                    spr[i].current_frame = sheet->count - 1;
                else
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
        if(asset_is_empty(spr[i].spritesheet))
            continue;

        sheet = asset_from_handle(spr[i].spritesheet);
        buf = get_frame(sheet, spr[i].current_frame);
        renderer_draw_sprite(sheet, buf, spr[i].rect);
    }
}
