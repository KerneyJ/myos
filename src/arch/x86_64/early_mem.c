#include "arch/x86_64/early_mem.h"

uint64_t init_earlymem(){
	/* initialize the page_bitmap to 0
	 * to map 512 GiB(size of pml3) is 4096 uint64_t */
	for(uint64_t i = 0; i < 4096; i++){
		page_bitmap[i] = 0;
	}

	/* mark first 2 pages as allocated(since they were allocated in boot.S) */
	page_bitmap[0] = 0x3;
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
	return PG_SIZE * (index * 64 + bit);// the base physical address of the page
}

uint64_t free_page_earlymem(uint64_t paddr){
	uint64_t index, bit;
	index = (paddr / PG_SIZE) / 64;
	bit = (paddr / PG_SIZE) % 64;
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
