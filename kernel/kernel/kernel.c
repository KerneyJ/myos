#include "multiboot.h"
#include "stdio.h"
#include "tty.h" 
#include "vm.h"

void kernel_main(uint32_t magic, uint32_t addr){
    /* Initialize terminal interface */
    term_init();

    if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
		printf("magic number does not match multiboot magic\n");
	}
	pg_init();
	printf("Hello World\n");
}
