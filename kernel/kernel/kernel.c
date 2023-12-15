#include "multiboot.h"
#include "stdio.h"
#include "tty.h"
#include "vm.h"
#include "arch.h"

void kernel_main(uint32_t magic, uint32_t addr){
	/*
	 * [+] success
	 * [-] error
	 * [*] info
	 */
	term_init(); printf("[+]Terminal Initialized\n");

	if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
		printf("[-]Magic number does not match multiboot magic\n");
	}
	pg_init(); printf("[+]Paging Initialized\n");
	int a20 = setup_arch(); printf("[*]Arch setup return %i\n", a20);
}
