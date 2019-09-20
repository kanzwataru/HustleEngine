#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

struct PlatformData;

struct Game {
    void (*init)(void);
    int  (*input)(void);
    void (*update)(void);
    void (*render)(void);
    void (*quit)(void);

    struct PlatformData *platform;
};

typedef int (*GameHandshadeFunction)(struct Game *game, void *memory);

#endif
