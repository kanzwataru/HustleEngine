#ifndef ENGINE_INTERNAL_H
#define ENGINE_INTERNAL_H
#include "common/platform.h"
#include "engine/engine.h"

/* timer_start maps directly to zen timer */
void zen_timer_stop(void);
unsigned int zen_timer_get();

#endif
