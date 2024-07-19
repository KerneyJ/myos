#include "arch/x86_64/early_mem.h"

/* from 0x200000 + 6 * PAGE_SIZE to 0x400000 + 6 * PAGE_SIZE */
uint64_t *page_bitmap = (uint64_t *)(MEM_BASE + 6 * PAGE_SIZE);

uint64_t earlymem_init(){
    /* Mark first 6 MiB as used in bitmap */
    for(int i = 0; i < 24; i++)
        page_bitmap[i] = 0xffffffffffffffff;
    return 0;
}

uint64_t alloc_page_earlymem(){
	uint64_t index, bit;
	for(index = 0; index < 4096; index++){
		for(bit = 0; bit < 64; bit++){
			if(!(page_bitmap[index] & (1 << bit)))
				break;
		}
	}
	return PAGE_SIZE * (index * 64 + bit);// the base physical address of the page
}

uint64_t free_page_earlymem(uint64_t paddr){
	uint64_t index, bit;
	index = (paddr / PAGE_SIZE) / 64;
	bit = (paddr / PAGE_SIZE) % 64;
	page_bitmap[index] = page_bitmap[index] & ~(1 << bit);
	return 0;
}

uint64_t map_page_earlymem(uint64_t vaddr, uint64_t paddr, uint64_t flags){
	/* Assumes 2 MiB page size
	 * TODO Make this coordinate with boot.S for abitrary page sizes
	 * Assumes that we are looking to extend pml2 since its early
	 */
	uint64_t pml3_index = (vaddr >> 29) & 0x1ff;
	uint64_t pml2_index = (vaddr >> 20) & 0x1ff;

	/*TODO check if pml2 at pml2_index index is present
	 * if not map it
	 */
	if(!(pml3[pml3_index] & 0x1)){
		uint64_t new_pml2_addr = alloc_page_earlymem();
		// TODO have to map this new page
		// TODO write this into the pml3 page table
	}

	if(!(pml2[pml2_index] & 0x1)){
	}
	return 0;
}
