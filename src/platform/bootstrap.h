#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include "common/platform.h"

struct Game {
    void (*init)(void);
    void (*input)(void);
    void (*update)(void);
    void (*render)(void);
    void (*quit)(void);

    bool running;
};

#define HANDSHAKE_FUNCTION_NAME game_loaded
typedef int (*game_handshake_func_t)(struct Game *game, void *memory);

#endif
