#ifndef EVENT_H
#define EVENT_H

#include "common/platform.h"

#define MAX_EVENTS 64
typedef unsigned char EventID;
typedef void (*EventCallback)(void *);

void event_update(void);
EventID event_add(EventCallback callback, void *arg, uint16 time_amount);
void event_remove(EventID id);

#endif
