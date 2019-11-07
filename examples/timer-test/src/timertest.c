#include "hustle.h"
#include "platform/bootstrap.h"

static struct Game *gametable;

static int spin_wheels(void)
{
    int i;
    volatile int foo = 1;
    volatile int *p_foo;

    for(i = 0; i < 3200 * 3200; ++i) {
        p_foo = &foo;
        *p_foo *= *p_foo;
    }

    return foo;
}

void init(void)
{
    double result;

    timer_start();
    spin_wheels();
    result = timer_stop_get_ms();

    printf("Speed is: %f ms\n", result);

    gametable->running = false;
}

void input(void) {}
void update(void) {}
void render(void) {}
void quit(void) {}

void HANDSHAKE_FUNCTION_NAME(struct Game *game, void *memory_chunk)
{
    gametable = game;

    game->init = init;
    game->input = input;
    game->update = update;
    game->render = render;
    game->quit = quit;
}
