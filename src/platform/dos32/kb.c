#include "platform/kb.h"

#define KEYBOARD_ISR        9   /* keyboard interrupt service number */
#define KEYCODES_MAX        255

#define TO_RELEASED(x)      ((x) | 0x80)  /* keys are OR'd with 0x80 if they are release codes */
#define FROM_RELEASED(x)    ((x) & ~0x80) /* get original (non-released) keycode */
#define IS_RELEASED(x)      ((x) & 0x80)  /* is the release bit set? */

static bool init_done = false;
static interrupt_t old_key_vect; /* the original keyboard interrupt service */
static bool key_array[KEYCODES_MAX];

/* GLOBALS */
bool const *keyboard_keys = key_array; /* global key array */
/* ******* */

static void __interrupt input_service(void)
{
    volatile byte key;

    //assert(init_done);
    _asm {
        cli
        push ax

        in  al,  60h    /* get key from keyboard input port */
        mov key, al

        in  al,  61h    /* send acknowledge to keyboard controller */
        or  al,  128
        out 61h, al
        xor al,  128
        out 61h, al

        pop ax
    }

    //*(char*)(0x0040001A) = 0x20; /* flush buffer */
    //*(char*)(0x0040001C) = 0x20; /*              */

    if(IS_RELEASED(key))
        key_array[FROM_RELEASED(key)] = false;
    else
        key_array[key] = true;

    //memset((void *)0xA0005, key, 40);

    outp(0x20, 0x20);
    _asm {
        //push ax
        //mov  al, 0x20
        //out  0x20, al

        //sti
        //iretd
    }
}

void keyboard_init(void)
{
    init_done = true;

    /* replace the keyboard interrupt service with our handler */
    old_key_vect = _dos_getvect(KEYBOARD_ISR);
    _dos_setvect(KEYBOARD_ISR, input_service);

    /* clear key array */
    memset(key_array, 0, KEYCODES_MAX);
}

void keyboard_quit(void)
{
    assert(init_done);
    init_done = false;

    /* bring back the original keyboard interrupt service */
    _dos_setvect(KEYBOARD_ISR, old_key_vect);
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
