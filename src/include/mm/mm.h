#ifndef MM_H
#define MM_H

#include <stddef.h>
#include <stdint.h>

#define NUM_BUCKETS 5
#define INIT_NUM_BLOCKS 8
#define INIT_SPACE (INIT_NUM_BLOCKS * (4096 + 1024 + 256 + 64 + 16) + NUM_BUCKETS * INIT_NUM_BLOCKS * sizeof(struct block))

// kalloc flags
#define KALLOC_ALIGNED 1 // ensure that address is page aligned

struct earlymem_info{
    uint64_t* page_bitmap;
    uint64_t bitmap_size; // size in bytes
    uint8_t log_page_size;
    uint8_t log_table_size;
    void* pml4;
    uint8_t* kernel_start;
    uint8_t* kernel_end;
};

struct block_header {
    union {
        struct {
            unsigned free : 1;
            unsigned aligned : 1;
            unsigned unused : 6;
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
void* kalloc(uint64_t size, int flags);
void* krealloc(void* ptr, uint64_t size, int flags);
void kfree(void *ptr);

#endif
