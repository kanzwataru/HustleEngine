#include "tmem.h"
#include "common/platform.h"
#include "platform/mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK_C_VAL     80
#define ASSERT_SLOTS_MATCH(x, y) \
    for(i = 0; i < MEM_BLOCK_SIZE; ++i) \
    { assert(((buffer_t *)mem_slot_get(x))[i] == ((buffer_t *)mem_slot_get(y))[i]) }

int test_mem(void)
{
    int i;
    memid_t block_a, block_b, block_c;
    buffer_t *slot_0_p, *slot_1_p;
    
    srand(clock());
    
    printf("initialize the memory manager\n");
    mem_init();
    
    printf("allocate two slots\n");
    mem_alloc_slot(0);
    mem_alloc_slot(1);
    slot_0_p = mem_slot_get(0);
    slot_1_p = mem_slot_get(1);
    
    printf("allocate two blocks\n");
    block_a = mem_alloc_block(0);
    block_b = mem_alloc_block(1);
    assert(block_a && block_b);
    
    printf("fill Block A (slot 0) with random\n");
    for(i = 0; i < MEM_BLOCK_SIZE; ++i) {
        slot_0_p[i] = rand();
    }
    
    printf("Block B is a clone of Block A\n");
    _fmemcpy(slot_1_p, slot_0_p, MEM_BLOCK_SIZE);
    ASSERT_SLOTS_MATCH(0, 1);
    
    printf("stow away Block B\n");
    mem_stash_block(block_b);
    
    printf("create Block C in Slot 1 and fill it with a single value\n");
    block_c = mem_alloc_block(1);
    assert(block_c);
    
    _fmemset(slot_1_p, BLOCK_C_VAL, MEM_BLOCK_SIZE);
    
    printf("stow away Block C\n");
    mem_stash_block(block_c);
    
    printf("bring back Block B and make sure it matches Block A\n");
    mem_restore_block(block_b, 1);
    ASSERT_SLOTS_MATCH(0, 1);
    
    printf("replace Block A with Block C and make sure it has the same contents as before\n");
    mem_restore_block(block_c, 0);
    for(i = 0; i < MEM_BLOCK_SIZE; ++i) {
        assert(slot_0_p[i] == BLOCK_C_VAL);
    }
    
    printf("free the slots\n");
    mem_free_slot(0);
    mem_free_slot(1);
    
    printf("quit the memory manager\n");
    mem_quit();
    
    printf("* MEM MANAGER TEST SUCCESS *\n");
    
    return 0;
}
