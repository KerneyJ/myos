#include "arch/x86_64/early_mem.h"

void init_freelist(){
}

uint64_t alloc_page(){
}

uint64_t map_page(uint64_t vaddr, uint64_t paddr, uint64_t flags){
	/* Assumes 2 MiB page size
	 * TODO Make this coordinate with boot.S for abitrary page sizes
	 */
	uint64_t pml4_index = vaddr >> 38;
	uint64_t pml3_index = (vaddr & 0x7fffffffff) >> 29;
	uint64_t pml2_index = (vaddr & 0x1fffff) >> 20;

	/*TODO check if pml4 at pml4_index index is present
	 * if not map it
	 */
	if(!(pml4[pml4_index] & 0x1)){
		pml4[pml4_index];
	}

	
	/*TODO check if pml3 at pml3_index index is present
	 * if not map it
	 */

	/*TODO check if pml2 at pml2_index index is present
	 * if not map it
	 */

}
