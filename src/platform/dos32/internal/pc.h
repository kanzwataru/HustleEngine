#ifndef PC_H
#define PC_H

#include "common/platform.h"

typedef void __interrupt __far(*interrupt_t)(void);

interrupt_t pc_getvect_dpmi(byte isr);
void pc_setvect_dpmi(byte isr, interrupt_t handler);

#endif
