#include "debug.h"
#include "platform.h"
#include "platform/video.h"

void debug_panic(void)
{
    printf("########### HUSTLE ENGINE PANIC ############\n");
    video_exit();
}

void debug_terminate(void)
{
    debug_panic();
    printf("TERMINATED\n");
    exit(1);
}
