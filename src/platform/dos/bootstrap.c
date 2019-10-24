#include "common/platform.h"
#include "platform/bootstrap.h"
#include "nativeplatform.h"

#include <stdlib.h>
#include <stdio.h>
#include <mem.h>

static struct PlatformData platform;
static struct Game game_table;
static void *memory = NULL;

extern int HANDSHAKE_FUNCTION_NAME(struct Game *game, void *memory);

static void cleanup(void)
{
    free(memory);
}

static void err(const char *err_msg)
{
    printf("Error: %s\n", err_msg);

    cleanup();
    exit(1);
}

int main(int argc, char **argv)
{
    memset(&game_table, 0, sizeof(game_table));
    memset(&platform, 0, sizeof(platform));
    
    memory = malloc(3000);
    if(!memory)
        err("Bad alloc");

    /* initialize game */
    game_table.platform = &platform;
    HANDSHAKE_FUNCTION_NAME(&game_table, memory);
    game_table.running = true;
    game_table.init();

    /* game loop */
    while(!kbhit()) {
        game_table.input();
        game_table.update();
        game_table.render();
    }

    game_table.quit();

    /* cleanup */
    cleanup();

    return 0;
}
