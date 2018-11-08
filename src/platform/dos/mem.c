#include "engine/mem.h"
#include "platform/common/memcmn.h"
#include "internal/xms.h"
#include "internal/biosmem.h"
#include "dosalloc.h"
#include <math.h>

static bool      xmspresent;
static uint32    extram_avail_kb;
static xmsid_t   stash_id;
static buffer_t *slotsraw; /* memory originally allocated */

static void init_segs(void)
{
    uint16 i;
    uint16 seg;
    
    DEBUG_DO(printf("Mem Manager: Initializing slot segments\n"));
    
    /* allocate enough for all slots */
    slotsraw = doscalloc(MEM_SLOT_MAX, MEM_BLOCK_SIZE);
    DEBUG_DO(printf("Slots base mem: %p\n", slotsraw));
    if(slotsraw == NULL) {
        PANIC("Not enough conventional memory available!");
    }
    
    seg = FP_SEG(slotsraw);
    
    /* assign slot variables */
    for(i = 0; i < MEM_SLOT_MAX; ++i) {
        slots[i] = MK_FP(seg, 0);
        DEBUG_DO(printf("   [%d] -> %p\n", i, slots[i]));
        
        seg += MEM_BLOCK_SIZE / 16;
    }
}

void mem_init(void)
{
    uint32 kb_required = (MEM_BLOCK_SIZE / 1024) * MEM_BLOCK_MAX;
    memset(slots, 0, MEM_SLOT_MAX); /* start out with NULL pointers */
    DEBUG_DO(printf("DOS Memory Management Init\n"));

    /* see whether to use XMS or the BIOS services */
    xmspresent = xms_query_installed();
    if(xmspresent) {
        extram_avail_kb = xms_query_avail();
        if(extram_avail_kb < kb_required) {
            PANIC("Not enough XMS RAM available!");
        }
        
        /* allocate one large flat chunk to manage here */
        stash_id = xms_alloc(kb_required);
    }
    else { /* using BIOS copy extended memory function */
        extram_avail_kb = bios_query_avail_extram();
        if(extram_avail_kb < kb_required) {
            PANIC("Not enough Extended Memory available! Try disabling EMS.");
        }
    }
    
    DEBUG_DO(printf("    Using XMS?: %d\n", xmspresent));
    DEBUG_DO(printf("    Free extended memory: %lu\n", extram_avail_kb));
    
    init_segs();
}

void mem_quit(void)
{
    dosfree(slotsraw);
    
    if(xmspresent) {
        xms_free(stash_id);
    }
}

void far *mem_slot_get(slotid_t slot)
{
    return slots[slot];
}

memid_t mem_alloc_block(slotid_t slot)
{
    memid_t new_block;
    
    /* evict whatever is in the slot if anything */
    evict_slot(slot);
    
    /* get a new block and slot it */
    new_block = find_empty_block();
    if(!new_block)
        PANIC("Mem Manager: Ran out of blocks\n");
    
    blocks[new_block].status = BLOCK_ACTIVE;
    blocks[new_block].slot   = slot;
    
    return new_block;
}

void mem_free_block(memid_t block)
{
    assert(block);
    
    /* don't actually free it, just mark as unallocated */
    blocks[block].status = BLOCK_NOT_ALLOCATED;
}

void mem_stash_block(memid_t block)
{
    assert(block);
    
    /* sanity check */
    if(blocks[block].status != BLOCK_ACTIVE &&
       blocks[block].status != BLOCK_ACTIVE_STASHED) 
    {
        fprintf(stderr, "\n\nMem Manager: Tried to stash block which isn't slotted: %d\n", block);
        dump_block(block);
        PANIC("MEMORY ERROR");
    }
    
    DEBUG_DO(printf("Mem Manager: stashing block %d in slot %d to extended memory...", block, blocks[block].slot));
    
    /* copy slotted memory to stash */
    if(xmspresent) {
        xms_copy_to_ext(stash_id, block * MEM_BLOCK_SIZE, slots[blocks[block].slot], MEM_BLOCK_SIZE);
    }
    else {
        bios_copy_to_ext(block * MEM_BLOCK_SIZE, slots[blocks[block].slot], MEM_BLOCK_SIZE);
    }
    
    if(blocks[block].status == BLOCK_ACTIVE)
        blocks[block].status = BLOCK_ACTIVE_STASHED;
        
    DEBUG_DO(printf("done.\n"));
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
    
    DEBUG_DO(printf("Mem Manager: restoring block %d in slot %d from extended memory...", block, slot));
    
    if(xmspresent) {
        xms_copy_from_ext(slots[slot], stash_id, block * MEM_BLOCK_SIZE, MEM_BLOCK_SIZE);
    }
    else {
        bios_copy_from_ext(slots[slot], block * MEM_BLOCK_SIZE, MEM_BLOCK_SIZE);
    }
    
    blocks[block].slot = slot;
    blocks[block].status = BLOCK_ACTIVE_STASHED;
    
    DEBUG_DO(printf("done.\n"));
}

