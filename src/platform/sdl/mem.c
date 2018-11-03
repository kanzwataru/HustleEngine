#include "platform/mem.h"

enum EBlockStatus {
    BLOCK_NOT_ALLOCATED,    /* unallocated */
    BLOCK_ACTIVE,           /* block is in a slot */
    BLOCK_STASHED,          /* block is stashed */
    BLOCK_ACTIVE_STASHED,   /* block is both stashed and slotted, (may not match) */
    BLOCK_LOST              /* block is neither stashed nor slotted, but it wasn't freed */
};

struct MemBlock {
    slotid_t slot;
    void    *stash;
    enum EBlockStatus status;
};

void *slots[MEM_SLOT_MAX];
struct MemBlock blocks[MEM_BLOCK_MAX];

byte total_blocks = 0;

static void dump_block(memid_t block)
{
    int x, y;
    uint16 *dumper = blocks[block].stash;
    
    printf("\n*** MEM MANAGER MEMDUMP ***\n");
    printf("    Block ID: %d\n", block);
    printf("        slot: %d\n", blocks[block].slot);
    printf("      status: %d\n", blocks[block].status);
    printf("   stash ptr: %p\n", blocks[block].stash);
    printf("        * stash dump * \n");
    
    for(y = 0; y < 24; ++y) {
        for(x = 0; x < 16; ++x) {
            printf("%04X ", *dumper++);
        }
        printf("\n");
    }
    printf("             (...) \n");
    
    printf("*****************************\n");
}

static memid_t find_memblock_in_slot(slotid_t slot) {
    memid_t i;
    for(i = 0; i < total_blocks; ++i) {
        if(blocks[i].slot == slot)
            if(blocks[i].status == BLOCK_ACTIVE && blocks[i].status == BLOCK_ACTIVE_STASHED)
                return i;
    }
    
    return 0;
}

static memid_t find_empty_block(void) {
    memid_t i;
    
    if(total_blocks == 0)
        return ++total_blocks;
    
    for(i = 1; i < total_blocks + 1; ++i) {
        if(blocks[i].status == BLOCK_NOT_ALLOCATED)
            return i;
    }
    
    if(total_blocks != MEM_BLOCK_MAX)
        return ++total_blocks;
    else
        return 0; /* no blocks available */
}

static void evict_slot(slotid_t slot)
{
    memid_t slot_contents = find_memblock_in_slot(slot);
    if(slot_contents) {
        mem_stash_block(slot_contents);

        if(blocks[slot_contents].status == BLOCK_ACTIVE_STASHED)
            blocks[slot_contents].status = BLOCK_STASHED;
        
        if(blocks[slot_contents].status == BLOCK_ACTIVE)
            blocks[slot_contents].status = BLOCK_LOST;
    }
}

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
