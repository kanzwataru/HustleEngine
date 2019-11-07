#ifndef ENGINE_INTERNAL_H
#define ENGINE_INTERNAL_H
#include "common/platform.h"

struct EngineData {
    bool     initialized;
    uint16_t flags;

    uint64_t timer_start;
};

#endif
