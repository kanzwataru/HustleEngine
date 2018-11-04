#ifndef BIOS_MEM_H
#define BIOS_MEM_H

uint16 bios_query_avail_extram(void);
void bios_copy_to_ext(uint32 offset, void far *memaddr, uint16 count);
void bios_copy_from_ext(void far *memaddr, uint32 offset, uint16 count);

#endif
