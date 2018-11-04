#include "platform/dos/internal/xms.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define XMS_ISR             0x2F
#define XMS_QUERY_EXISTS    0x4300
#define XMS_QUERY_DRIVER    0x4310

#define XMS_ALLOC           0x09
#define XMS_FREE            0x0A
#define XMS_COPY            0x0B
#define XMS_QUERY_AVAIL     0x08

struct XMSPage {
    xmsid_t handle;
    uint32  offset;
};

struct XMSCopyTable {
	uint32         len;
	struct XMSPage src;
	struct XMSPage dest;
};

static uint32 xmsdriver;
static uint16 xmstotal;

static void locate_driver_address(void)
{
    _asm {
        mov ax, XMS_QUERY_DRIVER
        int XMS_ISR
        mov [WORD PTR xmsdriver], bx
        mov [WORD PTR xmsdriver + 2], es
    }
}

bool xms_query_installed(void)
{
    union REGS r;
    bool installed;
    
    r.x.ax = XMS_QUERY_EXISTS;
    int86(XMS_ISR, &r, &r);
    
    installed = (r.h.al == 0x80);
    if(installed) {
        locate_driver_address();
    }
    
    return installed;
}

uint32 xms_query_avail(void)
{
    assert(xmsdriver);
    
    _asm {
        mov  ah, XMS_QUERY_AVAIL
        call [DWORD PTR xmsdriver]
        mov  xmstotal, ax
    }
    
    return xmstotal;
}

xmsid_t xms_alloc(uint16 kilobytes)
{
    xmsid_t id;
    uint16 success; 
    byte   err_code;
    
    _asm {
        mov  dx, kilobytes      /* load the desired size */
        
        mov  ah, XMS_ALLOC
        call [DWORD PTR xmsdriver]
        
        mov  id, dx             /* get the ID */
        mov  success, ax        /* get whether there was an error */
        mov  err_code, bl       /* get the error code */
    }
    
    if(!success) {
        printf("XMS Alloc Failed: %d\n", err_code);
        PANIC("MEMORY ERROR");
    }
    else {
        return id;
    }
}

void xms_free(xmsid_t id)
{
    byte  err_code;
    
    _asm {
        mov  dx, id
        
        mov  ah, XMS_FREE
        call [DWORD PTR xmsdriver]
        
        mov  err_code, bl
    }
    
    if(err_code) {
        printf("XMS Free Failed: %d\n", err_code);
        PANIC("MEMORY ERROR");
    }
}

void xms_copy_submit(struct XMSCopyTable *xct)
{
    uint16 x_seg, x_off, d_seg, d_off;
    uint16 success;
    byte   err_code;
    
    d_seg = FP_SEG(&xmsdriver);
    d_off = FP_OFF(&xmsdriver);
    x_seg = FP_SEG(xct);
    x_off = FP_OFF(xct);
    
    _asm {
        push es
        push ds
        push si
        push di
        
        mov  es, d_seg          /* load driver address */
        mov  di, d_off
        
        mov  ds, x_seg          /* load submission struct address */
        mov  si, x_off
        
        mov  ah, XMS_COPY       /* select copy function */
        call [DWORD PTR es:di]  /* call the driver */
        
        mov  success, ax
        mov  err_code, bl
        
        pop  di
        pop  si
        pop  ds
        pop  es
    }

    if(!success) {
        printf("XMS Memory Move Error: %d\n", err_code);
        PANIC("MEMORY ERROR");
    }
}

void xms_copy_to_ext(xmsid_t id, uint32 offset, void far *memaddr, uint32 count)
{
    struct XMSCopyTable xct;
    
    memset(&xct, 0, sizeof(struct XMSCopyTable));
    
    xct.len = TO_EVEN(count);
    xct.src.handle  = 0;
    xct.src.offset  = (uint32)memaddr;
    xct.dest.handle = id;
    xct.dest.offset = offset;
    
    xms_copy_submit(&xct);
}

void xms_copy_from_ext(void far *memaddr, xmsid_t id, uint32 offset, uint32 count)
{
    struct XMSCopyTable xct;
    
    memset(&xct, 0, sizeof(struct XMSCopyTable));
    
    xct.len = TO_EVEN(count);
    xct.src.handle  = id;
    xct.src.offset  = offset;
    xct.dest.handle = 0;
    xct.dest.offset = (uint32)memaddr;
    
    xms_copy_submit(&xct);
}
