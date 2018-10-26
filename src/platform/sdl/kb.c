#include "platform/kb.h"
#include "internal/sdl_shim.h"

static bool should_quit = false;

/* GLOBALS */
const bool *keyboard_keys; /* this will point to SDL's internal key array */
/* ******* */

void keyboard_per_frame_update(void) 
{
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                should_quit = true;
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == KEY_F4 && event.key.keysym.mod & KMOD_LALT)
                    should_quit = true;
                
                break;
        }
    }
    
    keyboard_keys = SDL_GetKeyState(NULL);
}

void keyboard_init(void)
{
    /* get a pointer to SDL's internal key state array */
    keyboard_keys = SDL_GetKeyState(NULL);
}

void keyboard_quit(void)
{
    /* no need to clean anything up */
}

bool keyboard_os_quit_event(void)
{
    return should_quit;
}
