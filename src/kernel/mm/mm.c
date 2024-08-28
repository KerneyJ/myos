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

static int prealloc_blocks(struct block* head, uint8_t* base, uint64_t block_size, uint64_t num_blocks, uint64_t blocks_per_page){
    struct block* node = head, *curr;
    uint8_t* bucket_base = base + sizeof(uint64_t);
    uint64_t idx, addr;

    for(int i = 0; i < num_blocks; i++) {
        // allocate a physical page
        addr = alloc_physpage(0);
        if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
            return -1;
        idx = hash(addr, blocks_per_page);
        curr = ((struct block*)bucket_base) + idx;
        while(curr->header.flags.present){
            bucket_base = ((uint64_t*)bucket_base - 1)[0]; // gross
            if(!bucket_base){
                addr = alloc_physpage(0);
            }
            if(curr->header.next){
                curr = curr->header.next;
                continue;
            }

        }
        if(curr->header.flags.present){
            // if this bucket is present then use the bucket on the adjacent page
            bucket_base = ((uint64_t*)base)[0];
            bucket_base += sizeof(uint64_t);
            curr = ((struct block*)bucket_base) + idx;
            if(curr->header.flags.present)
                return -1; // TODO future I should allocate another page?
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
        bucket_base = base + sizeof(uint64_t);
    }
    return 0;
};


int mem_init(struct earlymem_info info){
    /* Need some address to write to for debugging */
    if(kpaging_init(info) < 0)
        panic("kpaging_init failed");

    struct block b, *node;
    uint64_t addr, bpp, pos, idx; // blocks per page,
    uint8_t *page_base, *bucket_base;
    page_size = 1 << info.log_page_size;
    size_t block_size = page_size;
    bpp = (page_size - sizeof(uint64_t*)) / sizeof(struct block);

    // allocate memory for init memory data structures
    addr = alloc_physpage(0);
    if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
        return -1;
    memset(addr, 0, page_size);
    page_base = (uint8_t *)addr;
    addr = alloc_physpage(0);
    if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
        return -1;
    memset(addr, 0, page_size);

    page_base = (uint8_t *)addr;
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
    // 4k
    prealloc_blocks(block_list[0], page_base, page_size, bpp / NUM_BUCKETS, bpp);
/*
    bucket_base = page_base + sizeof(uint64_t);
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
    block_size /= 4;
*/
    return 0;
}

void* kalloc(uint64_t size, int flags){
    struct block* node;
    if(block_list[0] == 0 && bytes_avail == 0)
        panic("mem_init not called");
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


