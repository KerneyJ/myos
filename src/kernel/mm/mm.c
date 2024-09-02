#include "mm/mm.h"
#include "mm/paging.h"
#include "string.h"
#include "panic.h"

static struct block *block_list[NUM_BUCKETS] = {0} ; // for 4k 1k 256b & 64b
static struct block *block_misc = 0; // for random sizes after combining blocks
static uint64_t bytes_avail = 0;
static uint64_t page_size = 0;
static uint64_t buckets = 0; // number of buckets per page
static uint8_t *bucket_base = 0; // base address of the first page

static inline uint64_t hash(const uint64_t x){
    /* https://stackoverflow.com/questions/11871245/knuth-multiplicative-hash */
    return ((x * 2654435769) >> (32 - 12)) % buckets;
}

static int create_block() {
}

static struct block *findcreate_slot(uint64_t addr, uint8_t* bbase) {
    struct block* curr;
    uint64_t idx;
    idx = hash(addr);
    curr = ((struct block*)bbase) + idx;
    while(curr->header.flags.present){
        // if there is another bucket, go to it
        if( *((uint64_t*)bbase - 1) ){
            bbase = *((uint64_t*)bbase - 1);
            bbase += sizeof(uint64_t);
            curr = ((struct block*)bbase) + idx;
            continue;
        }

        // since there isn't another bucket, create it
        uint8_t *new_page =  alloc_gdpage();
        memset(new_page, 0, page_size); // zero out the page
        memcpy((uint64_t*)bbase - 1, &new_page, sizeof(uint64_t)); // move the address of the next page to the current;
        new_page += sizeof(uint64_t);
        curr = ((struct block*)new_page) + idx;
    }

    return curr;
}

static int prealloc_blocks(struct block* head, uint8_t* base, uint64_t block_size, uint64_t num_blocks){
    struct block* node = head, *curr;
    uint8_t* bbase = base + sizeof(uint64_t);
    uint64_t idx, addr;

    for(int i = 0; i < num_blocks; i += page_size / block_size){
        // allocate a physical page for the new blocks
        addr = alloc_gdpage();
        for(int j = 0; j < page_size / block_size; j++){
            curr = findcreate_slot(addr + j * block_size, bbase);
            // fill out block
            curr->header.iflags = 0;
            curr->header.flags.present = 1;
            curr->header.size = block_size;
            curr->header.prev = node;
            curr->header.next = 0;
            curr->addr = addr;
            node->header.next = curr;
            node = curr;
        }
        bbase = base + sizeof(uint64_t);
    }
    return 0;
};


int mem_init(struct earlymem_info info){
    /* Need some address to write to for debugging */
    if(kpaging_init(info) < 0)
        panic("kpaging_init failed");

    struct block b, *node;
    uint64_t addr, bpp, pos, idx; // blocks per page,
    page_size = 1 << info.log_page_size;
    size_t block_size = page_size;
    buckets = (page_size - sizeof(uint64_t*)) / sizeof(struct block);

    // allocate memory for init memory data structures
    addr = alloc_gdpage();
    memset(addr, 0, page_size);
    bucket_base = (uint8_t *)addr;
    pos += sizeof(uint64_t);

    // create headers
    for(int i = 0; i < NUM_BUCKETS; i++){
        b.header.iflags = 0;
        b.header.flags.present = 1;
        b.header.size = block_size;
        b.header.prev = bucket_base + pos;
        b.header.next = 0;
        b.addr = 0;
        memcpy(bucket_base + pos, &b, sizeof(struct block));
        block_list[i] = bucket_base + pos;
        pos += sizeof(struct block);
        block_size /= 4;
    }

    // create initial blocks
    block_size = page_size;
    for(int i = 0; i < NUM_BUCKETS; i++){
        prealloc_blocks(block_list[0], bucket_base, block_size, buckets / NUM_BUCKETS);
        block_size /= 4;
        bytes_avail += block_size * (bpp / NUM_BUCKETS);
    }
    return 0;
}

void* kalloc(uint64_t size){
    struct block* node, *prev, *next;
    void* ret = 0;
    int flags = 0x3; // for free and present
    if(block_list[0] == 0 && bytes_avail == 0)
        panic("mem_init not called");

    if(size > page_size) // In future this will be gone
        panic("cannot allocate more than page_size bytes yet");

    if(size > 1024 && size <= page_size){
        node = block_list[0];
    }
    else if(size > 256 && size <= 1024){
        node = block_list[1];
    }
    else if(size > 64 && size <= 256){
        node = block_list[2];
    }
    else if(size < 64){
        node = block_list[3];
    }
    else{
        return (void*)0;
    }

    for(; node != 0; node = node->header.next){
        if(node->header.iflags == flags){
            node->header.flags.free = 0;
            prev = node->header.prev;
            next = node->header.next;
            prev->header.next = next;
            if(next)
                next->header.prev = prev;
            ret = node->addr;
            bytes_avail -= node->header.size;
            break;
        }
    }

    return ret;
}

void* krealloc(void* ptr, uint64_t size, int flags){
    panic("Not implemented");
}

void kfree(void *ptr){
    struct block *curr, *head, *next;
    uint64_t idx = hash(ptr);
    uint8_t* bbase;

    // find the header
    bbase = bucket_base + sizeof(uint64_t);
    curr = ((struct block*)bbase) + idx;
    while( curr->addr != ptr ){
        bbase = *((uint64_t*)bbase - 1);
        if(!bbase)
            panic("couldn't find header");// TODO Should I panic here?
        bbase += sizeof(uint64_t);
        curr = ((struct block*)bbase) + idx;
    }

    // get the head node
    if(curr->header.size == page_size){
        head = block_list[0];
    }
    else if(curr->header.size == 1024){
        head = block_list[1];
    }
    else if(curr->header.size == 256){
        head = block_list[2];
    }
    else if(curr->header.size == 64){
        head = block_list[3];
    }
    else{
        panic("couldn't find head"); // TODO Should I panic here?
    }

    // append freed block
    next = head->header.next;
    next->header.prev = curr;
    head->header.next = curr;
    curr->header.flags.free = 1;
    bytes_avail += curr->header.size;
}
