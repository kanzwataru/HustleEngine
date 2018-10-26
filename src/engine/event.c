#include "engine/event.h"
#include <stdio.h>

struct Event {
    EventCallback  callback;
    void          *arg;
    uint16           timer;
};

static struct Event event_stack[MAX_EVENTS];
static EventID top;
static EventID last_freed;

EventID event_add(EventCallback callback, void *arg, uint16 time_amount)
{
    EventID ret_id;
    struct Event e;
    e.callback = callback;
    e.arg = arg;
    e.timer = time_amount;

    if(last_freed) {
        event_stack[last_freed] = e;
        ret_id = last_freed;
        last_freed = 0;
    }
    else {
        event_stack[++top] = e;
        assert(top < MAX_EVENTS);

        ret_id = top;
    }

    return ret_id;
}

void event_remove(EventID id)
{
    assert(id < top);
    memset(event_stack + id, 0, sizeof(struct Event));

    if(id == top - 1)
        --top;
    else
        last_freed = id;
}

void event_update(void)
{
    EventID i;

    for(i = 0; i < top; ++i) {
        if(event_stack[i].timer-- == 0) {
            if(event_stack[i].callback)
                event_stack[i].callback(event_stack[i].arg);

            event_remove(i);
        }
    }
}
