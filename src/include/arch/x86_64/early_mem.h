#ifndef EARLY_MEM_H
#define EARLY_MEM_H

#include <stdint.h>
#include "mm/mm.h"

/* Since we're in 64bit mode each should have 512 entries
 */
#define LOG_PAGE_SIZE 12
#define LOG_TABLE_SIZE 9
#define PAGE_SIZE (1 << LOG_PAGE_SIZE)
#define TABLE_SIZE (1 << LOG_TABLE_SIZE)
#define MEM_BASE 0x200000
#define EARLY_PT_BASE 0x600000
#define BITMAP_ENTRIES 0x8000
#define BITMAP_ENTRY_SIZE 8

extern uint64_t *pml4;
extern uint64_t *pml3;
extern uint64_t *pml2;
extern uint8_t* kernel_start;
extern uint8_t* kernel_end;

extern uint64_t swap_pagetables(uint64_t pml4);

uint64_t earlymem_init(struct earlymem_info* info);
uint64_t map_page_earlymem(uint64_t vaddr, uint64_t paddr, uint64_t flags);
uint64_t alloc_pagetable_earlymem();
uint64_t alloc_page_earlymem(uint64_t addr);
uint64_t free_page_earlymem(uint64_t paddr);

#endif
