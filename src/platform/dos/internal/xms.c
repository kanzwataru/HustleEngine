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

static volatile uint32 xmsdriver;
static volatile uint16 xmstotal;

static void locate_driver_address(void)
{
    volatile uint32 driver;

    _asm {
        mov ax, XMS_QUERY_DRIVER
        int XMS_ISR
        mov [WORD PTR driver], bx
        mov [WORD PTR driver + 2], es
    }

    xmsdriver = driver;
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
    volatile uint32  driver = xmsdriver;
    assert(xmsdriver);

    _asm {
        pusha

        mov  ah, XMS_QUERY_AVAIL
        call [DWORD PTR driver]
        mov  xmstotal, ax

        popa
    }

    return xmstotal;
}

xmsid_t xms_alloc(uint16 kilobytes)
{
    volatile xmsid_t id;
    volatile uint16  success;
    volatile byte    err_code;
    volatile uint32  driver = xmsdriver;

    _asm {
        pusha

        mov  dx, kilobytes      /* load the desired size */

        mov  ah, XMS_ALLOC
        call [DWORD PTR driver]

        mov  id, dx             /* get the ID */
        mov  success, ax        /* get whether there was an error */
        mov  err_code, bl       /* get the error code */

        popa
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
    volatile uint16  success;
    volatile byte    err_code;
    volatile uint32  driver = xmsdriver;

    _asm {
        pusha

        mov  dx, id

        mov  ah, XMS_FREE
        call [DWORD PTR driver]

        mov  success, ax        /* get whether there was an error */
        mov  err_code, bl       /* get the error code */

        popa
    }

    if(!success) {
        printf("XMS Free Failed: %d\n", err_code);
        PANIC("MEMORY ERROR");
    }
}

void xms_copy_submit(struct XMSCopyTable *xct)
{
    volatile uint32 driver = xmsdriver;
    volatile uint16 success;
    volatile byte   err_code;

    _asm {
        pusha

        lds  si, xct             /* load copy table */

        mov  ah, XMS_COPY        /* select copy function */
        call [DWORD PTR driver]  /* call the driver */

        mov  success, ax
        mov  err_code, bl

        popa
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
