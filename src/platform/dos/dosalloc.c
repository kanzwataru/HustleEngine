#include "dosalloc.h"
#include "common/platform.h"
#include "common/math.h"

#define DOS_ISR     0x21
#define DOS_ALLOC   0x48
#define DOS_FREE    0x49

void far *dosalloc(unsigned long size)
{
    union REGS r;
    
    r.x.bx = DIV_CEIL(size, 16);
    r.h.ah = DOS_ALLOC;
    int86(DOS_ISR, &r, &r);
    
    printf("-> %004X\n", r.x.ax);
    
	if(r.x.cflag)
        return 0;
    else
        return MK_FP(r.x.ax, 0);
}

void far *doscalloc(unsigned long count, unsigned long size)
{
    void far *ptr = dosalloc(count * size);
    if(!ptr)
        return 0;
        
    _fmemset(ptr, 0, count * size);
    
    return ptr;
}

void dosfree(void far *ptr)
{
    unsigned int segment = FP_SEG(ptr);
    
    if(!ptr)
        return;
        
    _asm {
        push es
        
        mov  es, segment
        mov  ah, DOS_FREE
        int  DOS_ISR
        
        pop  es
    }
}
