#include "engine/mem.h"
#include "common/math.h"

#define CHUNK_SIZE      512 /* bytes maximum size of chunk */
#define CHUNK_MAX       124 /* MEM_BLOCK_MAX / CHUNK_SIZE minus one which is needed for the pool header */
#define POOL_PTR(i)     (poolraw + ((i) * CHUNK_SIZE))
#define PTR_TO_ID(i)    (((unsigned char far *)(i)) - poolraw) / CHUNK_SIZE

#define USED_BLOCK      0
enum MCHUNK_FLAGS {
    CHNK_FREE = 0x01,   /* 0001 - whether the chunk is free */
    CHNK_HEAD = 0x02,   /* 0010 - whether this is the first multi-chunk */
    CHNK_TAIL = 0x04,   /* 0100 - whether this is the end of the multi-chunk */
    CHNK_TOP  = 0x08    /* 1000 - if all chunks after this are free */
};

typedef uint16 chunkid_t;
struct PoolHeader {
    memid_t blockid;
    byte    chunks[CHUNK_MAX]; /* array of MCHUNK_FLAGS for each chunk */
};

static unsigned char far     *poolraw = NULL;
static struct PoolHeader far *pool = NULL;

static bool find_free(chunkid_t num, chunkid_t *ret_head, chunkid_t *ret_tail)
{
    chunkid_t i, t;
    
    if(num > CHUNK_MAX)
        return false;
    
    for(i = 0; i < CHUNK_MAX; ++i) 
    {
        if((pool->chunks[i] & CHNK_TOP) &&
           (pool->chunks[i] & CHNK_FREE))
            goto only_one;
        
        if((pool->chunks[i] & CHNK_HEAD) && 
           (pool->chunks[i] & CHNK_FREE))
        {
            if(num == 1)
                goto only_one;
            
            t = i;
            while(t++ < CHUNK_MAX)
            {
                if(pool->chunks[t] & CHNK_TAIL) 
                {
                    if(t - i >= num)
                        goto both;
                    else
                        break; /* keep searching */
                }
            }
        }
    }
    
    return false; /* out of mem ? */
    
only_one:
    *ret_head = i;
    *ret_tail = 0;

    return true;
both:
    *ret_head = i;
    *ret_tail = i + num - 1;

    return true;
}

void mem_pool_init(slotid_t slot)
{
    memid_t block;
    if(poolraw)
        PANIC("Pool Manager: Double initialization!");
    
    /* get us some memory */
    block = mem_alloc_block(slot);
    pool = (struct PoolHeader *)((unsigned char far *)mem_slot_get(slot) + CHUNK_SIZE);
    assert(pool);
    
    /* start the actual pool right after the pool header chunk */
    poolraw = (unsigned char far *)pool + CHUNK_SIZE;
    
    /* initialize the pool */
    _fmemset(pool->chunks, CHNK_FREE, CHUNK_MAX);
    pool->chunks[0] = CHNK_TOP | CHNK_FREE;
    pool->blockid = block;
}

void mem_pool_quit(void)
{
    mem_free_block(pool->blockid);
    
    poolraw = NULL;
    pool = NULL;
}

void far *mem_pool_alloc(size_t size)
{
    chunkid_t i, head, tail, num_chunks;
    
    num_chunks = DIV_CEIL(size, 512);
    printf("Chunks to alloc: %d\n", num_chunks);
    if(!find_free(num_chunks, &head, &tail)) {
        DEBUG_DO(fprintf(stderr, "Pool Manager: Couldn't allocate %zu bytes\n", size));
        return NULL;
    }
    
    if(num_chunks == 1) {
        if(pool->chunks[head] & CHNK_TOP)
            pool->chunks[head + 1] |= CHNK_TOP;
        else
            pool->chunks[head + 1] |= CHNK_HEAD;

        pool->chunks[head] = CHNK_HEAD | CHNK_TAIL;
    }
    else {
        if(pool->chunks[head] & CHNK_TOP) {
            tail = head + num_chunks - 1;
            
            pool->chunks[tail + 1] |= CHNK_TOP;
        }
        else {
            pool->chunks[tail + 1] |= CHNK_HEAD;
        }
        
        for(i = head; i < tail; ++i)
            pool->chunks[i] = USED_BLOCK;
        pool->chunks[head] = CHNK_HEAD;
        pool->chunks[tail] = CHNK_TAIL;
    }
    
    return POOL_PTR(head);
}

void far *mem_pool_calloc(size_t num, size_t size)
{
    void far *ptr = mem_pool_alloc(num * size);
    
    _fmemset(ptr, 0, num * size);
    
    return ptr;
}

void mem_pool_free(void far *ptr)
{
    chunkid_t head, tail;
    head = PTR_TO_ID(ptr);
    
    if(pool->chunks[head] & CHNK_FREE)
        return; /* nothing to do, already freed */
    
    if((pool->chunks[head] & CHNK_HEAD) && 
       (pool->chunks[head] & CHNK_TAIL)) {
        /* this is only one chunk */
        pool->chunks[head] |= CHNK_FREE;
    }
    else {
        tail = head;
        while(tail++ < CHUNK_MAX) {
            /* find the tail and free everything in it's path */
            pool->chunks[tail] |= CHNK_FREE;
            
            if(pool->chunks[tail] & CHNK_TAIL) {
                /* found it! */
                return; /* we're done here */
            }
        }
        
        /* we've gone to the end without finding a tail */
        PANIC("POOL MANAGER: Couldn't find tail to free!");
    }
}
