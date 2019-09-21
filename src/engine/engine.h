#ifndef ENGINE_H
#define ENGINE_H

#include "common/platform.h"

enum ESubsystems {
    SUBSYS_RENDERER,
};

void engine_init(PlatformData *pd, enum ESubsystems subsystems);
void engine_reloaded(PlatformData *pd);
void engine_quit(PlatformData *pd);

#endif
