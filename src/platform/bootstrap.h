#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include "common/platform.h"

#define HANDSHAKE_FUNCTION_NAME game_loaded
typedef struct PlatformData PlatformData;

struct Game {
    void (*init)(void);
    bool (*input)(void);
    void (*update)(void);
    void (*render)(void);
    void (*quit)(void);

    bool running;
    bool initialized;

    PlatformData *platform;
};

typedef int (*GameHandshadeFunction)(struct Game *game, void *memory);

#endif
