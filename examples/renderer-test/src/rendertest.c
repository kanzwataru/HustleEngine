#include "hustle.h"
#include "platform/bootstrap.h"

#include <math.h>

struct GameData {
    struct Game *game;
    buffer_t palette[PALETTE_COLORS * 3];

    uint16_t counter;
    Rect bouncing_rect;
};

static struct GameData *g;

void init(void)
{
    int i;

    /* initialize palette */
    for(i = 0; i < PALETTE_COLORS * 3; i += 3) {
        /* using overflow on purpose */
        g->palette[i + 0] = i;
        g->palette[i + 1] = i + 64;
        g->palette[i + 2] = i + 128;
    }

    renderer_set_palette(g->palette, 0, 255);

    /* initialize scene */
    g->bouncing_rect.w = 32;
    g->bouncing_rect.h = 32;
}

void input(void) {}
void update(void)
{
    g->counter += 1;

    g->bouncing_rect.x = 32;
    g->bouncing_rect.y = 64 + (32.0f * sin((float)g->counter * 0.05f));
}

void render(void)
{
    renderer_clear(0);

    renderer_draw_rect(g->bouncing_rect, 18);
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
