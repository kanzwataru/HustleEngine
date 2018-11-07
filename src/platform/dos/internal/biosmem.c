#include "platform/dos/internal/biosmem.h"

#define BIOS_ISR            0x15
#define EXTCPY_SERVICE      0x87
#define EXTQUERY_SERVICE    0x88
#define EXT_MEM_START       0x100000L

#define PHYSADDR(x)         (((long)FP_SEG((x)) << 4) + FP_OFF((x)))

struct Desc {
    uint16  len;
    byte    addr[3];
    byte    access_rights;
    uint16  null;
};

struct GdTable {
    byte        unused[16];
    struct Desc src;
    struct Desc dest;
    byte        bios_use[16];
};

static uint32 extsize;

uint16 bios_query_avail_extram(void)
{
    union REGS r;
    
    r.h.ah = EXTQUERY_SERVICE;
    int86(BIOS_ISR, &r, &r);
    
    printf("EXTSIZE(): %d\n", r.x.ax);
    extsize = (r.x.ax + 1024L) * 1024; /* stored in bytes */
    
    return r.x.ax; /* KB */
}

static int extcpy(uint32 dst, uint32 src, uint16 count)
{
    struct GdTable gdt;
    union REGS r;
    struct SREGS s;
    
    if(count = 0)
        return -1;
    
    if(count >= extsize)
        return 4;
    
    /* clear structure to ensure bios-use areas are zeroed out */
    memset(&gdt, 0, sizeof(struct GdTable));
    
    /* source descriptor */
    gdt.src.len = count;
    gdt.src.addr[0] = src;
    gdt.src.addr[1] = src >> 8;
    gdt.src.addr[2] = src >> 16;
    gdt.src.access_rights = 0x93;
    
    /* destination descriptor */
    gdt.dest.len = count;
    gdt.dest.addr[0] = dst;
    gdt.dest.addr[1] = dst >> 8;
    gdt.dest.addr[2] = dst >> 16;
    gdt.dest.access_rights = 0x93;
    
    r.h.ah = EXTCPY_SERVICE;
    r.x.cx = count / 2; /* word count */
    s.es   = FP_SEG(&gdt);
    r.x.si = FP_OFF(&gdt);
    
    int86x(BIOS_ISR, &r, &r, &s);
    
    return(r.h.ah);
}

void bios_copy_to_ext(uint32 offset, void far *memaddr, uint16 count)
{
    extcpy(EXT_MEM_START + offset, PHYSADDR(memaddr), count);
}

void bios_copy_from_ext(void far *memaddr, uint32 offset, uint16 count)
{
    extcpy(PHYSADDR(memaddr), EXT_MEM_START + offset, count);
}
