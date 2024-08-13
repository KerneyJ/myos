#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include "mm/mm.h"

#define PG_BIG 0x80
#define PG_PRESENT 0x01
#define PG_WRITABLE 0x02

int kpaging_init(struct earlymem_info info);
uint64_t alloc_physpage(uint64_t paddr);
void free_page(uint64_t paddr);
int map_page(uint64_t vaddr, uint64_t paddr, uint64_t flags);
int unmap_page(uint64_t vaddr);
uint64_t create_pagetable();

#endif
