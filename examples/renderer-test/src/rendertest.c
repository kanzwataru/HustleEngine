#include "assets_main.gen.h"
#include "core/asset.h"
#include "hustle.h"
#include "platform/bootstrap.h"

#include <math.h>

struct GameData {
    struct Game *game;
    buffer_t palette[PALETTE_COLORS * 3];

    uint16_t counter;
    Rect bouncing_rect;
    Rect spinning_rect;
    Rect roy_rect;

    buffer_t test_texture[16 * 16];

    byte asset_pak[512000];
};

static struct GameData *g;

void init(void)
{
    int i;
    struct TextureAsset *roy;

    /* load pak file */
    asset_load_pak(g->asset_pak, "main.dat");
    roy = asset_get(ROY, Texture, g->asset_pak);

    /* initialize palette */
    for(i = 0; i < PALETTE_COLORS * 3; i += 3) {
        /* using overflow on purpose */
        g->palette[i + 0] = i;
        g->palette[i + 1] = i + 64;
        g->palette[i + 2] = i + 128;
    }

    renderer_set_palette(g->palette, 0, 255);

    /* initialize test texture */
    for(i = 0; i < PALETTE_COLORS; ++i) {
        g->test_texture[i] = i;
    }

    /* initialize scene */
    g->bouncing_rect.w = 32;
    g->bouncing_rect.h = 32;
    g->spinning_rect.w = 16;
    g->spinning_rect.h = 16;
    g->roy_rect.x = 8;
    g->roy_rect.y = 8;
    g->roy_rect.w = roy->width;
    g->roy_rect.h = roy->height;
}

void input(void) {}
void update(void)
{
    g->counter += 1;

    g->bouncing_rect.x = 32;
    g->bouncing_rect.y = 64 + (32.0f * sin((float)g->counter * 0.05f));

    g->spinning_rect.x = 128 + (16.0f * sin((float)g->counter * 0.01f));
    g->spinning_rect.y = 64 + (16.0f * cos((float)g->counter * 0.01f));
}

void render(void)
{
    struct TextureAsset *roy = asset_get(ROY, Texture, g->asset_pak);

    renderer_clear(30);

    renderer_draw_texture(roy->data, g->roy_rect);
    renderer_draw_rect(g->bouncing_rect, 12);
    renderer_draw_texture(g->test_texture, g->spinning_rect);

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
}
