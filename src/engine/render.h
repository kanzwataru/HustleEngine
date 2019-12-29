#ifndef RENDER_H
#define RENDER_H

#include "common/platform.h"
#include "common/mathlib.h"
#include "core/asset_structs.h"

#define PALETTE_COLORS  256

void renderer_clear(byte clear_col);
void renderer_flip(void);
void renderer_set_palette(const buffer_t *pal, byte offset, byte count);
void renderer_get_palette(buffer_t *pal, byte offset, byte count);

void renderer_draw_rect(byte color, Rect xform);
void renderer_draw_line(byte color, const Point *line, size_t count);
void renderer_draw_texture(const struct TextureAsset *texture, Rect xform);
void renderer_draw_sprite(const struct SpritesheetAsset *sheet, const buffer_t *frame, Rect xform);
void renderer_draw_tilemap(const struct TilemapAsset *map, const struct TilesetAsset *tiles, Point offset);
void renderer_draw_text(const struct FontAsset *font, const char *str, byte color, Rect bounds);

void renderer_clear_cache(void);

#endif
