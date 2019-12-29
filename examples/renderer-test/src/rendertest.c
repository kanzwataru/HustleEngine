#include "assets_main.gen.h"
#include "hustle.h"
#include "platform/bootstrap.h"

#include <math.h>
#include <stddef.h>

struct GameData {
    struct Game *game;
    buffer_t palette[PALETTE_COLORS * 3];

    uint16_t counter;
    Rect bouncing_rect;
    Rect spinning_rect;
    Rect roy_rect;

    struct Sprite sprites[6];
    Point tile_offset;
    Point tile_offset_dirs;

    byte test_texture_data[sizeof(struct TextureAsset) + (16 * 16)];
    byte test_font_atlas[sizeof(struct TextureAsset) + (256 * 128)];

    byte asset_pak[512000];
};

static struct GameData *g;
static const Point line_segs[] = {
    {32, 64}, {64, 128},
    {64, 128}, {48, 16},
    {48, 16}, {32, 8},
    {32, 8}, {-16, -24}
};

void init(void)
{
    int i;
    struct TextureAsset *roy;
    struct PaletteAsset *pal;

    roy = asset_get(ROY, Texture, g->asset_pak);

    /* initialize palette */
    for(i = 0; i < PALETTE_COLORS * 3; i += 3) {
        /* using overflow on purpose */
        g->palette[i + 0] = i;
        g->palette[i + 1] = i + 64;
        g->palette[i + 2] = i + 128;
    }

    //renderer_set_palette(g->palette, 0, 255);
    pal = asset_get(PAL, Palette, g->asset_pak);
    renderer_set_palette(pal->data, 0, pal->col_count);

    /* initialize test texture */
    ((struct TextureAsset *)&g->test_texture_data)->width = 16;
    ((struct TextureAsset *)&g->test_texture_data)->height = 16;
    ((struct TextureAsset *)&g->test_texture_data)->id = 1000;
    for(i = 0; i < PALETTE_COLORS; ++i) {
        g->test_texture_data[offsetof(struct TextureAsset, data[0]) + i] = i;
    }

    /* initialize test font atlas */
    ((struct TextureAsset *)&g->test_font_atlas)->width = 256;
    ((struct TextureAsset *)&g->test_font_atlas)->height = 128;
    ((struct TextureAsset *)&g->test_font_atlas)->id = 1001;
    struct FontAsset *font = asset_get(MED_FONT, Font, g->asset_pak);
    memcpy(((struct TextureAsset *)&g->test_font_atlas)->data, font->data, 256 * 128);

    /* initialize scene */
    g->bouncing_rect.w = 32;
    g->bouncing_rect.h = 32;
    g->spinning_rect.w = 16;
    g->spinning_rect.h = 16;
    g->roy_rect.x = 8;
    //g->roy_rect.y = 8;
    g->roy_rect.w = roy->width;
    g->roy_rect.h = roy->height;

    g->sprites[0].spritesheet = asset_handle_to(CHAR_RUN, Spritesheet, g->asset_pak);
    g->sprites[0].current_frame = 0;
    g->sprites[0].rect.y = 64;
    g->sprites[0].rect.w = asset_from_handle_of(g->sprites[0].spritesheet, Spritesheet)->width;
    g->sprites[0].rect.h = asset_from_handle_of(g->sprites[0].spritesheet, Spritesheet)->height;

    g->sprites[1].spritesheet = asset_handle_to(LARGE_RUN, Spritesheet, g->asset_pak);
    g->sprites[1].current_frame = 2;
    g->sprites[1].rect.x = 64;
    g->sprites[1].rect.y = 128;
    g->sprites[1].rect.w = asset_from_handle_of(g->sprites[1].spritesheet, Spritesheet)->width;
    g->sprites[1].rect.h = asset_from_handle_of(g->sprites[1].spritesheet, Spritesheet)->height;

    g->sprites[2] = g->sprites[1];
    g->sprites[2].rect.x += 32;

    g->sprites[3] = g->sprites[2];
    g->sprites[3].rect.x += 32;

    g->sprites[4] = g->sprites[3];
    g->sprites[4].rect.x += 32;

    g->sprites[5] = g->sprites[4];
    g->sprites[5].rect.x += 32;

    g->tile_offset.y = 128;
    g->tile_offset_dirs.x = -1;
    g->tile_offset_dirs.y = 1;
}

void input(void) {}
void update(void)
{
    int t_width;
    int t_height;
    struct TilemapAsset *map = asset_get(CITY_BG, Tilemap, g->asset_pak);

    t_width = map->tile_size * map->width;
    t_height = map->tile_size * map->height;

    g->counter += 1;

    g->bouncing_rect.x = 32;
    g->bouncing_rect.y = 64 + (32.0f * sin((float)g->counter * 0.05f));

    g->spinning_rect.x = 128 + (16.0f * sin((float)g->counter * 0.01f));
    g->spinning_rect.y = 64 + (16.0f * cos((float)g->counter * 0.01f));

    g->roy_rect.y = 8 + (4.0f * sin((float) g->counter * 0.05f));

    g->sprites[0].rect.x = 200 + (8.0f * sin((float)g->counter * 0.02f));
    g->sprites[1].rect.y = 200 - 32;

    /*
    g->sprites[2].rect.w = 16 - fabs(16.0f * sin((float)g->counter * 0.02f));
    g->sprites[3].rect.w = g->sprites[2].rect.w;
    g->sprites[3].rect.x = 128 + (15 - g->sprites[3].rect.w);
    g->sprites[4].rect.h = 32 - fabs(32.0f * sin((float)g->counter * 0.02f));
    g->sprites[5].rect.h = g->sprites[4].rect.h;
    g->sprites[5].rect.y = 128 + (32 - g->sprites[5].rect.h);
    */

    g->tile_offset.x += g->tile_offset_dirs.x;
    g->tile_offset.y += g->tile_offset_dirs.y;
    if(g->tile_offset.x > t_width - 320)
        g->tile_offset_dirs.x = -1;
    if(g->tile_offset.x < 0)
        g->tile_offset_dirs.x = 1;
    if(g->tile_offset.y > t_height - 200)
        g->tile_offset_dirs.y = -1;
    if(g->tile_offset.y < 0)
        g->tile_offset_dirs.y = 1;

    sprite_update(&g->sprites[0], 2);
}

void render(void)
{
    struct TextureAsset *roy = asset_get(ROY, Texture, g->asset_pak);
    renderer_clear(30);

    renderer_draw_tilemap(asset_get(CITY_BG, Tilemap, g->asset_pak), asset_get(STREET, Tileset, g->asset_pak), g->tile_offset);
    renderer_draw_texture(roy, g->roy_rect);
    renderer_draw_rect(12, g->bouncing_rect);
    renderer_draw_texture((struct TextureAsset *)(&g->test_texture_data), g->spinning_rect);
    sprite_draw(&g->sprites[0], 2);
    renderer_draw_line(16, line_segs, sizeof(line_segs) / sizeof(Point));

    /* NOTE: temp */
    renderer_draw_texture((struct TextureAsset *)(&g->test_font_atlas), (Rect){0, 0, 256, 128});
    
    renderer_flip();
}

void quit(void) {}

void HANDSHAKE_FUNCTION_NAME(struct Game *game, void *memory_chunk)
{
    g = (struct GameData *)memory_chunk;
    g->game = game;

    game->init = init;
    game->input = input;
    game->update = update;
    game->render = render;
    game->quit = quit;

    /* load pak file */
    asset_load_pak(g->asset_pak, "main.dat");
}
