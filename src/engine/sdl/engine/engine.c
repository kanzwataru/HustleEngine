#include "internal.h"
#include "engine/engine.h"
#include "engine/render.h"
#include "platform/sdl/nativeplatform.h"

static struct PlatformData *platform;
static struct EngineData *engine;

void engine_init(struct PlatformData *pd, enum ESubsystems subsystems)
{
    platform = pd;
    engine = &platform->engine;
    engine->subsystems = subsystems;

    //if(engine->subsystems & SUBSYS_RENDERER)
        renderer_init(pd);
}

void engine_reloaded(struct PlatformData *pd)
{
    platform = pd;
    engine = &platform->engine;

    //if(engine->subsystems & SUBSYS_RENDERER)
        renderer_reloaded(pd);
}

void engine_quit(struct PlatformData *pd)
{
    //if(engine->subsystems & SUBSYS_RENDERER)
        renderer_quit(pd);
}
