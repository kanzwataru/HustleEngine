#ifndef ENGINE_INTERNAL_H
#define ENGINE_INTERNAL_H
#include "common/platform.h"

struct EngineData {
    bool   initialized;
    uint16 flags;

    unsigned long long timer_start;
};

#endif
