#include "common/platform.h"
#include "engine/init.h"
#include "engine/input.h"
#include "platform/bootstrap.h"
#include "nativeplatform.h"

#include <stdlib.h>
#include <stdio.h>
#include <mem.h>

static struct PlatformData platform;
static struct Game game_table;
static void *memory = NULL;

extern int HANDSHAKE_FUNCTION_NAME(struct Game *game, void *memory);

FILE *logdev; /* global */

static void cleanup(void)
{
    engine_quit(&platform);
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

    memory = malloc(8000000); /* 8MB for now */
    if(!memory)
        err("Bad alloc");

    logdev = fopen("log.txt", "w");
    assert(logdev);

    /* initialize game */
    HANDSHAKE_FUNCTION_NAME(&game_table, memory);
    engine_init(&platform);
    game_table.running = true;
    game_table.init();

    /* game loop */
    while(!kbhit() && game_table.running) {
        if(keyboard_keys[KEY_ESC]) {
            game_table.running = false;
        }
        game_table.input();
        game_table.update();
        game_table.render();
    }

    game_table.quit();

    /* cleanup */
    cleanup();

    fclose(logdev);

    return 0;
}
