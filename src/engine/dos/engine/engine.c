#include "internal.h"
#include "engine/init.h"
#include "engine/render.h"
#include "platform/dos/nativeplatform.h"

#include <dos.h>

/* input */
#define KEYBOARD_ISR    9
#define KEYCODES_MAX    255

static interrupt_t default_input_isr;   /* pointer to original keboyard ISR */
static bool key_array[KEYCODES_MAX];
const *keyboard_keys = key_array;       /* GLOBAL */
/* */

static void __interrupt input_isr(void)
{
    volatile byte key;

    _asm {
        cli
        push ax

        in   al, 0x60   /* get key from keyboard input port */
        mov  key, al

        in   al, 0x61   /* send acknowledge to keyboard controller */
        or   al, 128
        out  0x61, al
        xor  al, 128
        out  0x61, al

        pop  ax

        sti
    }

    if(key & 0x80) {
        key_array[(key & ~0x80)] = false;
    }
    else {
        key_array[key] = true;
    }

    outp(0x20, 0x20);   /* send acknowledge */
}

void engine_init(struct PlatformData *pd)
{
    renderer_init(pd);

    /* replace the keyboard ISR */
    default_input_isr = _dos_getvect(KEYBOARD_ISR);
    _dos_setvect(KEYBOARD_ISR, input_isr);

    /* clear key array */
    memset(key_array, 0, KEYCODES_MAX);
}

void engine_quit(struct PlatformData *pd)
{
    renderer_quit(pd);

    /* put back keyboard interrupt service */
    _dos_setvect(KEYBOARD_ISR, default_input_isr);
}

double timer_stop_get_ms(void)
{
    long result;

    zen_timer_stop();
    result = zen_timer_get();
    if(result == 0) {
        return -1;
    }

    return (double)result * 0.001;
}
