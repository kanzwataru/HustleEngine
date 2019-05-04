#include "engine/core.h"
#include "engine/render.h"
#include "engine/asset.h"
#include "platform/filesys.h"

/* engine data */
static RenderData *rd;
static void *block_a_mem = NULL;
/* */

/* runtime data */
static byte col = 0;
/* */

static bool input(void)
{
    /* on modern systems, allow closing the window */
    if(keyboard_os_quit_event() || keyboard_keys[KEY_ESC])
        return false;

    /* the game loop can continue */
    return true;
}

static void update(void)
{

}

static void render(void)
{
    FILL_BUFFER(rd->screen, col++);
}

static void render_flip(void)
{
    /* finish up the frame and flip the screen buffer */
    renderer_finish_frame(rd);
}

static void quit(void)
{
    /* quit the renderer (switch back to text mode on DOS or close the window on modern PCs)
     * and engine */
    renderer_quit(rd, true);
    engine_quit();
}

void assettest_start(void)
{
    CoreData cd;

    /* initialize the engine's systems */
    engine_init();

    /* get a 64kb memory block */
    block_a_mem = malloc(64000);

    /* initialize the renderer with no sprites and no background inside block_a */
    rd = renderer_init(block_a_mem, 0, 0, DEFAULT_VGA_PALETTE);

    /* set up the default function table (input, update, etc...)
       and start the gameloop
    */
    ENGINE_DEFAULT_CALLBACKS(cd)
    engine_gameloop(cd);
}
