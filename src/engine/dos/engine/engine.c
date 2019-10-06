#include "internal.h"
#include "engine/render.h"
#include "platform/dos/nativeplatform.h"

void engine_init(PlatformData *pd, enum ESubsystems subsystems)
{
    renderer_init(pd);
}

void engine_reloaded(PlatformData *pd)
{

}

void engine_quit(PlatformData *pd)
{
    renderer_quit(pd);
}

