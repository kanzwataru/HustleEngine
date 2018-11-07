#ifndef DOS_ALLOC_H
#define DOS_ALLOC_H

void far *dosalloc(unsigned long size);
void far *doscalloc(unsigned long count, unsigned long size);
void dosfree(void far *ptr);

#endif
