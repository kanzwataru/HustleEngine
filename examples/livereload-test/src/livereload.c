#include "common/structures.h"
#include "platform/bootstrap.h"
#include "engine/engine.h"
#include "engine/render.h"

#include <math.h>

struct GameData {
    Rect a;
    int  counter;
};

static struct GameData *g;

void init(void) {
    g->a.x = 20;
    g->a.y = 40;
    g->a.w = 64;
    g->a.h = 64;

    g->counter = 0;
}

bool input(void) {
    return true;
}

void update(void) {
    g->a.x = (sin((float)g->counter++) * 0.5 + 0.5) * 320;
    g->a.y = (cos((float)g->counter++) * 0.5 + 0.5) * 320;
}

void render(void) {
    renderer_clear(16);
    renderer_draw_rect(0, g->a, 64);

    renderer_flip();
}

void quit(void) {

}

void HANDSHAKE_FUNCTION_NAME(struct Game *game, void *memory_chunk) {
    g = (struct GameData *)memory_chunk;

    game->init = init;
    game->input = input;
    game->update = update;
    game->render = render;
    game->quit = quit;

    if(!game->initialized) {
        engine_init(game->platform, SUBSYS_RENDERER);
        game->initialized = true;
    }
    else {
        engine_reloaded(game->platform);
    }
}
