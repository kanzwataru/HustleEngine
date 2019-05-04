#include "common/platform.h"
#include "engine/core.h"
#include "engine/render.h"

#include "platform/video.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static RenderData *rd;
static memid_t tblock = 0;
static void *transientmem = NULL;

static volatile unsigned char *vga = (unsigned char *)0xA0000;

static bool input(void)
{
    if(keyboard_os_quit_event() || keyboard_keys[KEY_ESC])
        return false;

    return true;
}

static void update(void)
{
    //printf("%d\n", rand());
}

static void render(void)
{
    int i;
    //renderer_start_frame(rd);
    /*
    for(i = 0; i < 320 * 200; ++i) {
        vga[i] = 6;
    } */
    memset((void*)vga, 4, 320 * 200);
}

static void quit(void)
{
    //renderer_quit(rd, true);
    free(transientmem);
}

static void render_flip(void)
{
    //renderer_finish_frame(rd);
}

static void modeset(byte mode) {
    _asm {
        pusha
        mov al, mode
        int 0x10
        popa
    }
}

void simpletest_start(void)
{
    //int i;
    //byte palette[256];

    //engine_init();
    //video_init_mode(VIDEO_MODE_LOW256, 1);
    modeset(0x13);
    //transientmem = malloc(64000);

    //srand(0);
    /*
    for(i = 0; i < 256; ++i) {
        palette[i] = rand();
    }
*/
    //rd = renderer_init(transientmem, 0, 0, 0);

    keyboard_init();
    memset((void *)0xA0000, 5, 320 * 200);


    while(input()) {
        //update();
        //render();
        //render_flip();
        vga[0] += 1;
    }


    modeset(0x03);

    keyboard_quit();

    //engine_quit();

    //quit();
}

// #include <stdio.h>
// #include <dos.h>
// #include <conio.h>
// #include <stdlib.h>
// #include <string.h>
//
// void main(void) {
//     _asm {
//         pusha
//         mov al, 0x13
//         int 0x10
//         popa
//     }
//
//     memset((void *)0xA0000, 5, 320 * 200);
//     getch();
//
//     _asm {
//         pusha
//         mov al, 0x03
//         int 0x10
//         popa
//     }
// }
