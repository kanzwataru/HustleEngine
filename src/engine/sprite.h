#ifndef SPRITE_H
#define SPRITE_H

#include "core/asset.h"
#include "common/mathlib.h"

struct Sprite {
    assetid_t spritesheet;
    int current_frame;
    int frameskip;  /* TODO: this ought to be global */
    Rect rect;
};

void sprite_update(struct Sprite *spr, size_t count);
void sprite_draw(struct Sprite *spr, size_t count);

#endif
