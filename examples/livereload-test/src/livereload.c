#include "common/structures.h"
#include "platform/bootstrap.h"
#include "engine/engine.h"
#include "engine/render.h"

#include <stdio.h>

struct GameData {
    Rect a;
    int  counter;
    Point dir;
};

static struct GameData *g;

void init(void) {
    g->a.x = 20;
    g->a.y = 40;
    g->a.w = 64;
    g->a.h = 64;

    g->counter = 0;
    
    g->dir.x = 1;
    g->dir.y = 1;
}

bool input(void) {
    return true;
}

void update(void) {
    g->a.x += g->dir.x;
    g->a.y += g->dir.y;
    
    if(g->a.x > 320 - g->a.w)
        g->dir.x = -1;
    if(g->a.x < 0)
        g->dir.x = 1;
    if(g->a.y > 200 - g->a.h)
        g->dir.y = -1;
    if(g->a.y < 0)
        g->dir.y = 1;
    
    //printf("%d %d\n", g->a.x, g->a.y);
}

void render(void) {
    renderer_clear(16);
    renderer_draw_rect(0, g->a, 255);

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
