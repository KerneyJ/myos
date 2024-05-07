#include "arch/x86_64/early_mem.h"

/* from 0x200000 + 6 * PAGE_SIZE to 0x400000 + 6 * PAGE_SIZE */
uint64_t *page_bitmap = (uint64_t *)(MEM_BASE + 6 * PAGE_SIZE);

uint64_t earlymem_init(){ /* FIXME */
	/* initialize the page_bitmap to 0
	 * to map 512 GiB(size of pml3) is 4096 uint64_t
	 * mark first 2 pages as allocated(since they were allocated in boot.S)
	 */
	uint64_t *new_pml4, *new_pml3, *new_pml2;
	new_pml4 = (uint64_t*)MEM_BASE; // set pml4 to 2 MiB
	new_pml3 = (uint64_t*)(MEM_BASE + PAGE_SIZE);
	new_pml2 = (uint64_t*)(MEM_BASE + 2 * PAGE_SIZE);
	for(uint64_t i = 0; i < TABLE_SIZE; i++)
		new_pml4[i] = 0x0;

	new_pml4[0] = (uint64_t)new_pml3 + PG_PRESENT + PG_WRITABLE;
	new_pml4[272] = (uint64_t)new_pml3 + PG_PRESENT + PG_WRITABLE;

	for(uint64_t i = 0; i < TABLE_SIZE; i++)
		new_pml3[i] = 0x0;

	new_pml3[0] = (uint64_t)new_pml2 + PG_PRESENT + PG_WRITABLE;

	for(uint64_t i = 0; i < TABLE_SIZE; i++)
		new_pml2[i] = 0x0;

	uint64_t *pml1_base = (uint64_t *)(MEM_BASE + 3 * PAGE_SIZE);
	new_pml2[0] = (uint64_t)pml1_base + PG_PRESENT + PG_WRITABLE;
	new_pml2[1] = (uint64_t)pml1_base + PAGE_SIZE + PG_PRESENT + PG_WRITABLE;
	new_pml2[2] = (uint64_t)pml1_base + 2*PAGE_SIZE + PG_PRESENT + PG_WRITABLE;
	for(uint64_t i = 0; i < 3 * TABLE_SIZE; i++)
		pml1_base[i] = 0x0;

	/* populate both pml1s
	 * assume that _kernel_end provided by linker scripter is page aligned
	 * map the kernel pages into the page tables
	 * for some reason _kernel_end(provided by linker) value is cooked; will just give kernel 16 pages */
	for(uint64_t i = 0; i < 16; i++)
		pml1_base[i] = (i * PAGE_SIZE) + PG_PRESENT + PG_WRITABLE;

	/* map the page tables and bitmap pages into the page tables
	 * memory information is located at 0x200000 to 0x404000*/
	uint64_t base_index = MEM_BASE / PAGE_SIZE;
	for(uint64_t i = 0; i < TABLE_SIZE + 6; i++)
		pml1_base[i+base_index] = MEM_BASE + (i * PAGE_SIZE) + PG_PRESENT + PG_WRITABLE;

	/* setup the bitmap
	 * First mark the first 16 pages
	 * Last mark the bits for pages 0x200000 to 0x406000 */
	page_bitmap[0] = 0xffff;
	base_index = (MEM_BASE / PAGE_SIZE) / 64;
	for(uint64_t i = base_index; i < (TABLE_SIZE / 64) + base_index; i++){
		page_bitmap[i] = 0xffffffffffffffff;
	}
	page_bitmap[base_index + (TABLE_SIZE / 64)] = 0x3f;

	swap_pagetables((uint64_t)new_pml4);
	pml4 = new_pml4;
	pml3 = new_pml3;
	pml2 = new_pml2;
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
