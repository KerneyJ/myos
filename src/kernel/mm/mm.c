#include "mm/mm.h"
#include "mm/paging.h"
#include "string.h"
#include "panic.h"

static struct block *block_list[NUM_BUCKETS] = {0} ; // for 4k 1k 256b 64b and 16b blocks
static struct block *block_misc = 0; // for random sizes after combining blocks
static uint64_t bytes_avail = 0;
static uint64_t page_size = 0;

static inline uint64_t hash(const uint64_t x){
    return ((x * 2654435769) >> (32 - 12)) % (sizeof(struct block) / page_size);
}


int mem_init(struct earlymem_info info){
    if(kpaging_init(info) < 0)
        panic();

    struct block b, *node;
    uint64_t addr, pos = 0;
    uint8_t *base;
    size_t block_size = page_size;
    page_size = 1 << info.log_page_size;

    // allocate memory for init memory data structures
    addr = alloc_physpage(0);
    if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
        return -1;
    base = (uint8_t *)addr;

    // create headers
    for(int i = 0; i < NUM_BUCKETS; i++){
        b.header.iflags = 0;
        b.header.size = block_size;
        b.header.prev = base + (i * sizeof(struct block));
        b.header.next = 0;
        b.addr = 0;
        memcpy(base + (i * sizeof(struct block)), &b, sizeof(struct block));
        block_size /= 4;
    }
    return 0;
}

void* kalloc(uint64_t size, int flags){
    struct block* node;
    if(block_list[0] == 0 && bytes_avail == 0)
        panic();
    // calulate index
    // check flags
    return (void*)0;
}

void* krealloc(void* ptr, uint64_t size, int flags){
    ;
}

void kfree(void *ptr){
    ;
}


