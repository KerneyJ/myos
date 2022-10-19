#include "vm.h"

uint32_t pgdir[1024] __attribute__((aligned(4096)));
uint32_t kernel_pgtbl[1024] __attribute__((aligned(4096)));

extern void pgdir_load(uint32_t*); // load page directory
extern void pg_enable(); // turn on paging

void pg_init(){
    pgdir_init();
    pgtbl_init();
	pgdir_load(pgdir);
	pg_enable();
}

void pgdir_init(){
    uint32_t i;
	for(i = 0; i < 1024; ++i){
		// supervisor mode, read write, not present
		pgdir[i] = PTE_RW;
	}
	// uint32_t *pgdir_loc = 0x100000;
	// *pgdir_loc = *pgdir;
}

void pgtbl_init(){
    uint32_t i;
	for(i = 0; i < 1024; ++i){
		kernel_pgtbl[i] = (i * 0x1000) | PTE_PRESENT | PTE_RW;
	}
	pgdir[0] = ((uint32_t)kernel_pgtbl) | PTE_PRESENT | PTE_RW;
}
