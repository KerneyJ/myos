#include "arch/x86_64/early_mem.h"

/* from 0x200000 + 6 * PAGE_SIZE to 0x400000 + 6 * PAGE_SIZE */
uint64_t *page_bitmap = (uint64_t *)(MEM_BASE + 6 * PAGE_SIZE);

uint64_t earlymem_init(){
    /* Mark first 6 MiB as used in bitmap */
    for(int i = 0; i < 24; i++)
        page_bitmap[i] = 0xffffffffffffffff;
    page_bitmap[24] = 0xfffffffffffffffe;
    /* map page for the framebuffer */
    /* return map_page_earlymem(0xfd000000, 0, 0); */
    uint64_t page = alloc_page_earlymem();
    page_bitmap[25] = page;
    return 0;
}

uint64_t alloc_page_earlymem(){
    /* In future maybe have static variable or smth that caches
     * the location of free pages
     * or have some sort of data structure to make this faster
     */
    uint64_t page, width = 64, mask = 0xffffffffffffffff, index;
    for(index = 0; index < 32768; index++){
        if(mask != page_bitmap[index]){
            break;
        }
    }
    page = page_bitmap[index];
    while(width > 1){
        if((mask & page) != mask){
            width /= 2;
            mask = mask & (mask << width);
        }
        else
            mask = mask >> width;
    }
    if((mask & page) == mask)
        mask >>= 1;
    return (index * 64 + mask) * PAGE_SIZE; // the base physical address of the page
}

uint64_t free_page_earlymem(uint64_t paddr){
	uint64_t index, bit;
	index = (paddr / PAGE_SIZE) / 64;
	bit = (paddr / PAGE_SIZE) % 64;
	page_bitmap[index] = page_bitmap[index] & ~(1 << bit);
	return 0;
}

uint64_t map_page_earlymem(uint64_t vaddr, uint64_t paddr, uint64_t flags){
    // writing stuff to page_bitmap[24] for debuging
    uint64_t pml4_index = (vaddr >> 39) & 0x1ff;
    uint64_t pml3_index = (vaddr >> 30) & 0x1ff;
    uint64_t pml2_index = (vaddr >> 21) & 0x1ff;
    uint64_t pml1_index = (vaddr >> 12) & 0x1ff;
    uint64_t *pml;

    pml = (uint64_t*)((uint64_t)pml4 & 0xfffffffff000);
    if(!(pml[pml4_index] & PG_PRESENT))
        return 4;

    pml = (uint64_t*)((uint64_t)pml3 & 0xfffffffff000);
    if(!(pml[pml3_index] & PG_PRESENT)){
        uint64_t temp_paddr = alloc_page_earlymem();
        page_bitmap[26] = temp_paddr;
        return 3;
    }

    pml = (uint64_t*)((uint64_t)pml2 & 0xfffffffff000);
    if(!(pml[pml2_index] & PG_PRESENT))
        return 2;

    return 0;
}
