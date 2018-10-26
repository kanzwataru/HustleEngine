#include "engine/core.h"
#include "common/platform.h"
#include "engine/event.h"
#include "platform/video.h"

void engine_start(CoreData cd)
{
    /* Game Loop */
    while(cd.input_handler()) 
    {
        cd.update_callback();
        event_update();

        cd.render_callback();
        video_wait_vsync();
        cd.flip_callback();
    }

    cd.exit_handler();
}
