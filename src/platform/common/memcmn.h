/*
 * Included in the same compilation unit as platform-specific mem.c files
*/
enum MBlockStatus {
    BLOCK_NOT_ALLOCATED,    /* unallocated */
    BLOCK_ACTIVE,           /* block is in a slot */
    BLOCK_STASHED,          /* block is stashed */
    BLOCK_ACTIVE_STASHED,   /* block is both stashed and slotted, (may not match) */
    BLOCK_LOST              /* block is neither stashed nor slotted, but it wasn't freed */
};

struct MemBlock {
    slotid_t slot;
    byte     status;
    void    *stash; /* not used on DOS */
};

static void *slots[MEM_SLOT_MAX];
static struct MemBlock blocks[MEM_BLOCK_MAX];

static byte total_blocks = 0;

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