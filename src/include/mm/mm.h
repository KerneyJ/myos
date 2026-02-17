#ifndef MM_H
#define MM_H

#include "mm/common.h"

#define NUM_BUCKETS 4

// kalloc flags
#define KALLOC_ALIGNED 1 // ensure that address is page aligned

struct block_header {
    union {
        struct {
            unsigned free : 1;
            unsigned present : 1;
            unsigned aligned : 1;
            unsigned unused : 5;
        } flags;
        uint8_t iflags;
    };
    uint64_t size;
    struct block* next;
    struct block* prev;
};

struct block{
    struct block_header header;
    void* addr;
};

int mem_init(struct earlymem_info info);
void* kalloc(uint64_t size);
void* krealloc(void* ptr, uint64_t size, int flags);
void kfree(void *ptr);

#endif
