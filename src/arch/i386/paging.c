#include "arch/i386/paging.h"

extern char _kernel_end[];
extern void pgdir_load(uint32_t*); // load page directory
extern void pg_enable(); // turn on paging

uint32_t pgdir[1024] __attribute__((aligned(PG_SIZE)));
uint32_t kernel_pgtbl[1024] __attribute__((aligned(PG_SIZE)));
// need to store 32768 uint32's to map 4G(32 pages) stored currently at 10Mib
uint32_t* pg_bitmap = (uint32_t *)10485760;// _kernel_end % PG_SIZE == 0 ? _kernel_end : (PG_SIZE - (_kernel_end % PG_SIZE)) + _kernel_end;

/* Invoke all functions involed in turning on paging
 */
void pg_init(){
	pgdir_init();
	pgtbl_init();
	pgdir_load(pgdir);
	pg_enable();
}

/* Initialize the top level page table(page directory)
 */
void pgdir_init(){
	uint32_t i;
	for(i = 0; i < 1024; ++i){
		// supervisor mode, read write, not present
		pgdir[i] = PTE_RW;
	}
}

/* Initialize the kernel page table as a directly mapped page table
 */
void pgtbl_init(){
	uint32_t i;
	for(i = 0; i < 1024; ++i){
		kernel_pgtbl[i] = (i * 0x1000) | PTE_PRESENT | PTE_RW;
	}
	pgdir[0] = ((uint32_t)kernel_pgtbl) | PTE_PRESENT | PTE_RW;
}

/* Initialize the bitmap that stores the page frames
 * Setting the first 10 MiB as used, will change
 * this in the future. Kernel should free some pages frames too.
 */
void pf_init(){
	uint32_t i;

	// Mark 10 MiB as used
	for(i = 0; i < 80; ++i){
		pg_bitmap[i] = 0xFFFFFFFF;
	}

	// Mark the next 32 pages as used(pages being used by pageframe bitmap)
	pg_bitmap[81] = 0xFFFFFFFF;
}
