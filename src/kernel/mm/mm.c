#include "mm/mm.h"
#include "mm/paging.h"
#include "string.h"
#include "panic.h"

static struct block *block_list[NUM_BUCKETS] = {0} ; // for 4k 1k 256b & 64b
static struct block *block_misc = 0; // for random sizes after combining blocks
static uint64_t bytes_avail = 0;
static uint64_t page_size = 0;

static inline uint64_t hash(const uint64_t x, const uint64_t buckets){
    /* https://stackoverflow.com/questions/11871245/knuth-multiplicative-hash */
    return ((x * 2654435769) >> (32 - 12)) % buckets;
}


int mem_init(struct earlymem_info info){
    /* Need some address to write to for debugging */
    if(kpaging_init(info) < 0)
        panic();

    struct block b, *node, *curr;
    uint64_t addr, bpp, bpb, pos, idx; // blocks per page, blocks per bucket
    uint8_t *page_base, *bucket_base;
    page_size = 1 << info.log_page_size;
    size_t block_size = page_size;
    bpp = (page_size - sizeof(uint64_t*)) / sizeof(struct block);
    bpb = bpp / NUM_BUCKETS;

    // allocate memory for init memory data structures
    addr = alloc_physpage(0);
    if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
        return -1;
    page_base = (uint8_t *)addr;
    addr = alloc_physpage(0);
    if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
        return -1;
    memcpy(page_base, &addr, sizeof(uint64_t));
    ((uint64_t*)addr)[0] = 0;
    pos += sizeof(uint64_t);

    // create headers
    for(int i = 0; i < NUM_BUCKETS; i++){
        b.header.iflags = 0;
        b.header.flags.present = 1;
        b.header.size = block_size;
        b.header.prev = page_base + pos;
        b.header.next = 0;
        b.addr = 0;
        memcpy(page_base + pos, &b, sizeof(struct block));
        block_list[i] = page_base + pos;
        pos += sizeof(struct block);
        block_size /= 4;
    }

    // create initial blocks
    block_size = page_size;
    bucket_base = page_base + sizeof(uint64_t);
    // 4k
    node = block_list[0];
    for(int j = 0; j < bpb; j++){
        // allocate a physical page
        addr = alloc_physpage(0);
        if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
            return -1;
        idx = hash(addr, bpp);
        curr = ((struct block*)bucket_base) + idx;
        if(curr->header.flags.present){
            // if this bucket is present then use the bucket on the adjacent page
            bucket_base = ((uint64_t*)page_base)[0];
            bucket_base += sizeof(uint64_t);
            curr = ((struct block*)bucket_base) + idx;
            if(curr->header.flags.present)
                return -2; // TODO future I should allocate another page?
        }

        // fill out block
        curr->header.iflags = 0;
        curr->header.flags.present = 1;
        curr->header.flags.aligned = 1;
        curr->header.size = block_size;
        curr->header.prev = node;
        curr->header.next = 0;
        curr->addr = addr;
        node->header.next = curr;
        node = curr;
        bucket_base = page_base + sizeof(uint64_t);
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


