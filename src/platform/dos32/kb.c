#include "platform/kb.h"

#define KEYBOARD_ISR        9   /* keyboard interrupt service number */
#define KEYCODES_MAX        255

#define TO_RELEASED(x)      ((x) | 0x80)  /* keys are OR'd with 0x80 if they are release codes */
#define FROM_RELEASED(x)    ((x) & ~0x80) /* get original (non-released) keycode */
#define IS_RELEASED(x)      ((x) & 0x80)  /* is the release bit set? */

static bool             init_done = false;
static void interrupt (*old_key_vect)(void); /* the original keyboard interrupt service */
static bool key_array[KEYCODES_MAX];

/* GLOBALS */
bool const *keyboard_keys = key_array; /* global key array */
/* ******* */

static void interrupt input_service(void)
{
    byte key;
    
    assert(init_done);
    _asm {
        in  al,  60h    /* get key from keyboard input port */
        mov key, al

        in  al,  61h    /* send acknowledge to keyboard controller */
        or  al,  128
        out 61h, al
        xor al,  128
        out 61h, al
    }

    *(char*)(0x0040001A) = 0x20; /* flush buffer */
    *(char*)(0x0040001C) = 0x20; /*              */

    if(IS_RELEASED(key))
        key_array[FROM_RELEASED(key)] = false;
    else
        key_array[key] = true;
    
    outportb(0x20, 0x20);
}

void keyboard_init(void)
{
    init_done = true;
    
    /* replace the keyboard interrupt service with our handler */
    old_key_vect = getvect(KEYBOARD_ISR);
    setvect(KEYBOARD_ISR, input_service);
    
    /* clear key array */
    memset(key_array, 0, KEYCODES_MAX);
}

void keyboard_quit(void)
{
    assert(init_done);
    init_done = false;
    
    /* bring back the original keyboard interrupt service */
    setvect(KEYBOARD_ISR, old_key_vect);
}

void keyboard_per_frame_update(void) 
{
    /* On DOS,
     * we use an interrupt so we won't poll what keys are there,
     * the hardware will tell us directly
    */
}

bool keyboard_os_quit_event(void)
{
    return false; /* DOS doesn't have an OS-level way of quitting */
}
