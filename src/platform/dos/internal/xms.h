#ifndef XMS_H
#define XMS_H
#include "common/platform.h"

typedef uint16 xmsid_t;

bool xms_query_installed(void);
uint32 xms_query_avail(void);

xmsid_t xms_alloc(uint16 kilobytes);
void xms_free(xmsid_t id);
void xms_copy_to_ext(xmsid_t id, uint32 offset, void far *memaddr, uint32 count);
void xms_copy_from_ext(void far *memaddr, xmsid_t id, uint32 offset, uint32 count);

#endif
