#ifndef ENGINE_INTERNAL_H
#define ENGINE_INTERNAL_H
#include "common/platform.h"

struct EngineData {
    uint16 flags;
    uint16 subsystems;

    unsigned long long timer_start;
};

#endif
