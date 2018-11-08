#ifndef MEM_H
#define MEM_H
#include "common/platform.h"
#include "common/layout.h"

#define MEM_BLOCK_SIZE      64000
#define MEM_BLOCK_MAX       128     /* temporarily hard-coded */
#define MEM_SLOT_MAX        6

typedef byte memid_t;
typedef byte slotid_t;

void mem_init(void);
void mem_quit(void);

void far *mem_slot_get(slotid_t slot);

memid_t mem_alloc_block(slotid_t slot);
void mem_free_block(memid_t block);
void mem_stash_block(memid_t block);
void mem_restore_block(memid_t block, slotid_t slot);

void mem_pool_init(slotid_t slot);
void mem_pool_quit(void);
void far *mem_pool_alloc(size_t size);
void far *mem_pool_calloc(size_t num, size_t size);
void mem_pool_free(void far *ptr);

#endif
