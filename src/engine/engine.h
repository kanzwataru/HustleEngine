#ifndef ENGINE_H
#define ENGINE_H

#include "common/platform.h"

enum ESubsystems {
    SUBSYS_RENDERER,
};

void engine_init(struct PlatformData *pd, enum ESubsystems subsystems);
void engine_reloaded(struct PlatformData *pd);
void engine_quit(struct PlatformData *pd);

#endif
