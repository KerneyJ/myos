#include "stdio.h"
#include "tty.h" 
#include "vm.h"

void kernel_main(void) 
{
    /* Initialize terminal interface */
    term_init();
	uint32_t cr3;
	asm volatile ("mov %%cr3, %0" : "=r"(cr3));
	printf("cr3: %x\n", cr3);
	pg_init();
	asm volatile ("mov %%cr3, %0" : "=r"(cr3));
	printf("cr3: %x\n", cr3);
}
