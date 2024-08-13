#include "mm/mm.h"
#include "mm/paging.h"
#include "string.h"
#include "panic.h"

static struct block *block_list[NUM_BUCKETS] = {0} ; // for 4k 1k 256b 64b and 16b blocks
static struct block *block_misc = 0; // for random sizes after combining blocks
static uint64_t bytes_avail = 0;

int mem_init(struct earlymem_info info){
    if(kpaging_init(info) < 0)
        panic();

    struct block b, *node;
    uint64_t addr, pos = 0;
    uint8_t *base;
    const size_t page_size = 1 << info.log_page_size;
    size_t block_size = page_size;

    // allocate memory for int memory data structures
    addr = alloc_physpage(0);
    if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
        return -1;
    base = (uint8_t *)addr;
    block_size = sizeof(struct block);
    memcpy(base, &block_size, sizeof(size_t));
/*
    for(uint64_t i = 0; i < INIT_SPACE; i += page_size){
        addr = alloc_physpage(0);
        if(addr == 0 || map_page(base + i * page_size, addr, PG_WRITABLE) < 0)
           return -1;
    }

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

    // preallocate blocks
    // 16b
    // 64b
    // 256b
    // 1k
    // 4k
*/
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


