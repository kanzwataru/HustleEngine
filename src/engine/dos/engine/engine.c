#include "internal.h"
#include "engine/init.h"
#include "engine/render.h"
#include "platform/dos/nativeplatform.h"

void engine_init(struct PlatformData *pd)
{
    renderer_init(pd);
}

void engine_quit(struct PlatformData *pd)
{
    renderer_quit(pd);
}

double timer_stop_get_ms(void)
{
    long result;

    zen_timer_stop();
    result = zen_timer_get();
    if(result == 0) {
        return -1;
    }

    return (double)result * 0.001;
}
