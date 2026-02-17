#ifndef PAGING_H
#define PAGING_H

#include "mm/common.h"

// pagetable_node: built to have a pool of page tables pre allocated
struct pt_node {
    uint64_t* addr;
    struct pt_node* next;
};

int kpaging_init(struct earlymem_info info);
uint64_t alloc_physpage(uint64_t paddr);
uint64_t alloc_big_physpage(uint64_t paddr);
uint64_t alloc_gdpage();
void free_page(uint64_t paddr);
int map_page(uint64_t vaddr, uint64_t paddr, uint64_t flags);
int unmap_page(uint64_t vaddr);
uint64_t alloc_pagetable();

#endif
