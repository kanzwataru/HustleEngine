#include "common/structures.h"
#include "platform/bootstrap.h"
#include "engine/engine.h"
#include "engine/render.h"

#include <stdio.h>
#include <stdlib.h>

#define DO_CUBE_TEST 1
#define CUBE_MAX     512
#define CUBE_DELAY   200

#define RAND_RANGE(_min, _max) ((rand() % ((_max) - (_min) + 1)) + (_min))

struct Cube {
    bool  showing;
    Rect  rect;
    Point dir;
};

struct GameData {
    struct Game *game;

    int  counter;
    int  cube_counter;
    int  cube_num;
    struct Cube cubes[CUBE_MAX];
};

static const buffer_t palette[] = {
    128, 70, 70,
    255, 128, 128,
};

static struct GameData *g;

void init(void) {
    int i = 0;

    for(i = 0; i < CUBE_MAX; ++i) {
        g->cubes[i].rect.x = RAND_RANGE(0, 128);
        g->cubes[i].rect.y = RAND_RANGE(0, 64);
        g->cubes[i].rect.w = 16;
        g->cubes[i].rect.h = 16;

        g->cubes[i].dir.x = RAND_RANGE(0, 1) ? -1 : 1;
        g->cubes[i].dir.y = RAND_RANGE(0, 1) ? -1 : 1;
    }

    g->cubes[0].showing = true;

    g->counter = 0;
    g->cube_counter = 0;
    g->cube_num = 1;
}

void input(void) {
}

void update(void) {
    int i;
    struct Cube *c;

#if DO_CUBE_TEST
    if(g->cube_counter++ > CUBE_DELAY) {
        g->cube_counter = 0;
        g->cube_num *= 2;
        if(g->cube_num > CUBE_MAX)
            g->cube_num = CUBE_MAX;
    }
#endif

    for(i = 0; i < g->cube_num; ++i) {
        c = &g->cubes[i];

        c->rect.x += c->dir.x;
        c->rect.y += c->dir.y;

        if(c->rect.x > 320 - c->rect.w)
            c->dir.x = -1;
        if(c->rect.x < 0)
            c->dir.x = 1;
        if(c->rect.y > 200 - c->rect.h)
            c->dir.y = -1;
        if(c->rect.y < 0)
            c->dir.y = 1;
    }

    //printf("%d %d\n", g->a.x, g->a.y);
}

void render(void) {
    int i = 0;

    renderer_set_palette(palette, 0, 2);
    renderer_clear(0);

    for(i = 0; i < g->cube_num; ++i) {
        renderer_draw_rect(0, g->cubes[i].rect, 1);
    }

    renderer_flip();
}

void quit(void) {
    engine_quit(g->game->platform);
}

void HANDSHAKE_FUNCTION_NAME(struct Game *game, void *memory_chunk) {
    g = (struct GameData *)memory_chunk;
    g->game = game;

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
