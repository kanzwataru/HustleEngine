#include "platform/mem.h"
#include "platform/common/memcmn.h"

void mem_init(void)
{
    int i;
    for(i = 0; i < MEM_SLOT_MAX; ++i) {
        slots[i] = calloc(1, MEM_BLOCK_SIZE);
        assert(slots[i]);
    }
    
    for(i = 0; i < MEM_BLOCK_MAX; ++i) {
        memset(&blocks[i], 0, sizeof(struct MemBlock));
        
        blocks[i].stash = calloc(1, MEM_BLOCK_SIZE);
        assert(blocks[i].stash);
    }
}

void mem_quit(void)
{
    int i;
    for(i = 0; i < MEM_SLOT_MAX; ++i) {
        free(slots[i]);
    }
    
    for(i = 0; i < MEM_BLOCK_MAX; ++i) {
        free(blocks[i].stash);
    }
}

void mem_alloc_slot(slotid_t slot)
{
    /* nothing to do on this platform,
     * it's all pre-allocated */
}

void mem_free_slot(slotid_t slot)
{
    /* nothing to do on this platform,
     * it's all pre-allocated */
}

memid_t mem_alloc_block(slotid_t slot)
{
    /* evict whatever is in the slot if anything */
    evict_slot(slot);
    
    /* get a new block and slot it */
    memid_t new_block = find_empty_block();
    if(!new_block)
        PANIC("Mem Manager: Ran out of blocks\n");
    
    blocks[new_block].status = BLOCK_ACTIVE_STASHED;
    blocks[new_block].slot   = slot;
    
    /* clear slot and new block stash
     *
     * This is not required and won't happen on DOS, but
     * it may help with debugging game code.
     */
    memset(slots[slot], 0, MEM_BLOCK_SIZE);
    memset(blocks[new_block].stash, 0, MEM_BLOCK_SIZE);
    
    return new_block;
}

void mem_free_block(memid_t block)
{
    /* don't actually free it, just mark as unallocated */
    blocks[block].status = BLOCK_NOT_ALLOCATED;
}

void mem_stash_block(memid_t block)
{
    /* sanity check */
    if(blocks[block].status != BLOCK_ACTIVE &&
       blocks[block].status != BLOCK_ACTIVE_STASHED) 
    {
        fprintf(stderr, "\n\nMem Manager: Tried to stash block which isn't slotted: %d\n", block);
        dump_block(block);
        PANIC("MEMORY ERROR");
    }
    
    /* copy slotted memory to stash */
    memcpy(blocks[block].stash, slots[blocks[block].slot], MEM_BLOCK_SIZE);
}

void mem_restore_block(memid_t block, slotid_t slot)
{
    /* sanity check */
    if(blocks[block].status != BLOCK_STASHED &&
       blocks[block].status != BLOCK_ACTIVE_STASHED)
    {
        fprintf(stderr, "\n\nMem Manager: Cannot restore unstashed block: %d to slot: %d\n", block, slot);
        PANIC("MEMORY ERROR");
    }
    
    evict_slot(slot);
    
    memcpy(slots[slot], blocks[block].stash, MEM_BLOCK_SIZE);
    blocks[block].slot = slot;
    blocks[block].status = BLOCK_ACTIVE_STASHED;
}

void far *mem_slot_get(slotid_t slot)
{
    return slots[slot];
}
