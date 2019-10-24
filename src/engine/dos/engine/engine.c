#include "internal.h"
#include "engine/render.h"
#include "platform/dos/nativeplatform.h"

void engine_init(struct PlatformData *pd, enum ESubsystems subsystems)
{
    renderer_init(pd);
}

void engine_reloaded(struct PlatformData *pd)
{

}

void engine_quit(struct PlatformData *pd)
{
    renderer_quit(pd);
}

