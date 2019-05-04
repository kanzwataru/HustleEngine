#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "common/platform.h"
#include "platform/kb.h"
////#include "platform/mem.h"

#define ENGINE_DEFAULT_CALLBACKS(cd)   \
    cd.update_callback = update;    \
    cd.render_callback = render;    \
    cd.flip_callback = render_flip; \
    cd.input_handler = input;       \
    cd.exit_handler = quit;

typedef struct {
    void (*update_callback)(void);
    void (*render_callback)(void);
    void (*flip_callback)(void);
    bool (*input_handler)(void);
    void (*exit_handler)(void);
    int frame_skip;
} CoreData;

void engine_init(void);
void engine_gameloop(CoreData cd);
void engine_gameloop_break(void);
void engine_quit(void);

#endif
