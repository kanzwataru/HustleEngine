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
