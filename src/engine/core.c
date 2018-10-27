#include "engine/core.h"
#include "common/platform.h"
#include "engine/event.h"
#include "platform/video.h"

static bool stop;

void engine_start(CoreData cd)
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

void engine_release(void)
{
    stop = true;
}
