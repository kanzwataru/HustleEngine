#ifndef ENGINE_H
#define ENGINE_H

#include "common/platform.h"

enum ESubsystems {
    SUBSYS_RENDERER,
};

void engine_init(struct PlatformData *pd, enum ESubsystems subsystems);
void engine_reloaded(struct PlatformData *pd);
void engine_quit(struct PlatformData *pd);

void timer_start(void);
double timer_stop_get_ms(void);

#endif
