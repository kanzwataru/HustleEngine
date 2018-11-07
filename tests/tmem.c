#include "tmem.h"
#include "common/platform.h"
#include "platform/mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BLOCK_C_VAL     80
#define ASSERT_SLOTS_MATCH(x, y) \
    for(i = 0; i < MEM_BLOCK_SIZE; ++i) \
    { assert(((buffer_t *)mem_slot_get(x))[i] == ((buffer_t *)mem_slot_get(y))[i]) }

#define PTROUT(x) printf(" -> %p\n", (x));

int test_mem_simple(void)
{
    char     *str = ";; This is the example data ;;\n";
    char far *slotptr;
    memid_t   block;
    
    printf("initialize memory manager\n");
    mem_init();
    
    printf("allocate a slot\n");
    mem_alloc_slot(0);
    slotptr = mem_slot_get(0);
    printf("    slot: %p\n", slotptr);
    
    printf("allocate a block\n");
    block = mem_alloc_block(0);
    printf("    block: %d\n", block);
    
    printf("write this string to the slot: %s\n", str);
    strcpy(slotptr, str);
    puts(str);
    puts(slotptr);
    assert(strcmp(slotptr, str) == 0);
    
    printf("stash block\n");
    mem_stash_block(block);
    
    printf("overwrite slot\n");
    _fmemset(slotptr, 'x', 8);
    puts(slotptr);
    
    printf("restore block\n");
    mem_restore_block(block, 0);

    printf("check the data\n");
    puts(str);
    puts(slotptr);
    assert(strcmp(slotptr, str) == 0);
    
    printf("free the block\n");
    mem_free_block(block);
    
    printf("free the slot\n");
    mem_free_slot(0);
    
    printf("quit the memory manager\n");
    mem_quit();

    printf("* MEM MANAGER SIMPLE TEST SUCCESS *\n");

#ifdef PLATFORM_DOS
    printf("\nPress any key to continue...\n");
    getch();
#endif
    
    return 0;
}

int test_mem_pool(void)
{
    void far *a, *b, *c, *d;
    
    printf("\n Memory Pool Test\n");
    
    printf("initialize the memory manager\n");
    mem_init();
    
    printf("initialize the memory pool manager\n");
    mem_pool_init(0);
    
    printf("alloc three 512-byte arrays\n");
    a = mem_pool_alloc(512);
    b = mem_pool_alloc(512);
    c = mem_pool_alloc(512);
    PTROUT(a);
    PTROUT(b);
    PTROUT(c);
    assert(a && b && c);
    assert(a != b && b != c && a != c);
    
    printf("alloc a 16k array\n");
    d = mem_pool_alloc(16 * 1024);
    PTROUT(d);
    assert(d);
    
    printf("free a 512-byte array\n");
    mem_pool_free(b);
    b = NULL;
    
    printf("alloc a 4k array\n");
    b = mem_pool_alloc(4 * 1024);
    PTROUT(b);
    assert(b);
    
    printf("free the 16k array\n");
    mem_pool_free(d);
    d = NULL;
    
    printf("alloc a 4k array\n");
    d = mem_pool_alloc(4 * 1024);
    PTROUT(d);
    assert(d);
    
    printf("free a 512-byte array\n");
    mem_pool_free(a);
    a = NULL;
    
    printf("alloc a 2k array\n");
    a = mem_pool_alloc(2 * 1024);
    PTROUT(a);
    assert(a);
    
    printf("free a 512-byte array\n");
    mem_pool_free(c);
    c = NULL;
    
    printf("alloc a 712-byte array (odd-sized)\n");
    c = mem_pool_alloc(712);
    PTROUT(c);
    assert(c);
    
    printf("quit the memory pool manager\n");
    mem_pool_quit();
    
    printf("* MEM POOL MANAGER TEST SUCCESS\n");
#ifdef PLATFORM_DOS
    printf("\nPress any key to continue...\n");
    getch();
#endif
    
    return 0;
}

int test_mem(void)
{
    unsigned int i;
    memid_t block_a, block_b, block_c;
    buffer_t *slot_0_p, *slot_1_p;
    
    test_mem_simple();
    
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
    for(i = 0; i < MEM_BLOCK_SIZE; ++i) {
        if(slot_1_p[i] != BLOCK_C_VAL) {
            printf("[%d] Non-matching value: %02X !%02X! %02X\n", i, slot_1_p[i - 1], slot_1_p[i], slot_1_p[i + 1]);
            return 1;
        }
    }
    
    printf("stow away Block C\n");
    mem_stash_block(block_c);
    
    printf("bring back Block B and make sure it matches Block A\n");
    mem_restore_block(block_b, 1);
    ASSERT_SLOTS_MATCH(0, 1);
    
    printf("replace Block A with Block C and make sure it has the same contents as before\n");
    mem_restore_block(block_c, 0);
    for(i = 0; i < MEM_BLOCK_SIZE; ++i) {
        //assert(slot_0_p[i] == BLOCK_C_VAL);
        if(slot_0_p[i] != BLOCK_C_VAL) {
            printf("[%d] Non-matching value: %02X !%02X! %02X\n", i, slot_0_p[i - 1], slot_0_p[i], slot_0_p[i + 1]);
            return 1;
        }
    }
    
    printf("free the slots\n");
    mem_free_slot(0);
    mem_free_slot(1);
    
    printf("quit the memory manager\n");
    mem_quit();
    
    printf("* MEM MANAGER TEST SUCCESS *\n\n");
    
    test_mem_pool();
    
    return 0;
}
