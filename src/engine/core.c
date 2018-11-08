#include "engine/core.h"
#include "common/platform.h"
#include "engine/event.h"
#include "platform/video.h"

static bool stop;
static bool initialized = false;

void engine_init(void)
{
    if(initialized)
        PANIC("HUSTLE ENGINE: Double core initialization");
    
    mem_init();
    mem_pool_init(MEMORY_POOL_MEMSLOT);
    keyboard_init();
    
    initialized = true;
}

void engine_gameloop(CoreData cd)
{
    stop = false;
    
    /* Game Loop */
    while(cd.input_handler()) 
    {
        if(stop)
            return;
            
        cd.update_callback();
        event_update();

        cd.render_callback();
        video_wait_vsync();
        cd.flip_callback();
    }

    cd.exit_handler();
}

void engine_gameloop_break(void)
{
    stop = true;
}

void engine_quit(void)
{
    keyboard_quit();
    mem_pool_quit();
    mem_quit();
}
