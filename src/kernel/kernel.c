#include "mb.h"
#include "stdio.h"
#include "tty.h"
#include "arch.h"
#include "mm/mm.h"
#include "video.h"

void kernel_main(uint32_t magic, uint32_t addr){
    /*
     * [+] success
     * [-] error
     * [*] info
     */
    struct earlymem_info einfo;
    struct video_info vinfo;

    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        goto failure;

    if (addr & 7) // unaligned multiboot info struct
        goto failure;

    configure_arch(&einfo);
    if(mem_init(einfo) < 0)
        panic("mem_init failed");
    mb_init(addr, &vinfo);

    return; // jump out of kernl_main to scheduler

failure:
	printf("failure\n");
	return;
}
