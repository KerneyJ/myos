#include "mb.h"
#include "stdio.h"
#include "tty.h"
#include "arch.h"
#include "mm/mm.h"


void kernel_main(uint32_t magic, uint32_t addr){
    /*
     * [+] success
     * [-] error
     * [*] info
     */
    struct earlymem_info info;
    void *fb;

    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        goto failure;

    if (addr & 7) // unaligned multiboot info struct
        goto failure;

    configure_arch(&info);
    if(mem_init(info) < 0)
        panic("mem_init failed");
    fb = mb_init(addr);

    return; // jump out of kernl_main to scheduler

failure:
	printf("failure\n");
	return;
}
