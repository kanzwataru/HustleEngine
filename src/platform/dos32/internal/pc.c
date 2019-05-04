#include "pc.h"

#define DPMI_GET_EXCEPT_VECTOR  0x202
#define DPMI_SET_EXCEPT_VECTOR  0x203
#define DPMI_GET_PM_ISR_VECTOR  0x204
#define DPMI_SET_PM_ISR_VECTOR  0x205

interrupt_t pc_getvect_dpmi(byte isr)
{
    volatile uint16 sel;
    volatile uint32 ofs;
    volatile uint32 function;

    /* the bottom interrupts are actually CPU exceptions */
    if(isr <= 1) {
        function = DPMI_GET_EXCEPT_VECTOR;
    }
    else {
        function = DPMI_GET_PM_ISR_VECTOR;
    }

    _asm {
        pusha

        mov eax, function
        mov bl,  isr
        int 0x31

        mov sel, cx
        mov ofs, edx

        popa
    }

    return MK_FP(sel, ofs);
}

void pc_setvect_dpmi(byte isr, interrupt_t handler)
{
    volatile uint16 sel;
    volatile uint32 ofs;
    volatile uint32 function;

    sel = FP_SEG(handler);
    ofs = FP_OFF(handler);

    /* the bottom interrupts are actually CPU exceptions */
    if(isr <= 1) {
        function = DPMI_SET_EXCEPT_VECTOR;
    }
    else {
        function = DPMI_SET_PM_ISR_VECTOR;
    }

    _asm {
        pusha

        mov eax, function
        mov edx, ofs
        mov cx,  sel
        mov bl,  isr
        int 0x31

        popa
    }
}
