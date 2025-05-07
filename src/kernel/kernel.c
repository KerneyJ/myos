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

    // Draw a line
    for (unsigned i = 0; i < vinfo.width && i < vinfo.height; i++){
        switch (vinfo.bpp){
            case 8:
                {
                    multiboot_uint8_t *pixel = vinfo.framebuffer_addr + vinfo.pitch * i + i;
                    *pixel = vinfo.color;
                }
                break;
            case 15:
            case 16:
                {
                    multiboot_uint16_t *pixel = vinfo.framebuffer_addr + vinfo.pitch * i + 2 * i;
                    *pixel = vinfo.color;
                }
                break;
            case 24:
                {
                    multiboot_uint32_t *pixel = vinfo.framebuffer_addr + vinfo.pitch * i + 3 * i;
                    *pixel = (vinfo.color & 0xffffff) | (*pixel & 0xff000000);
                }
                break;
            case 32:
                {
                    multiboot_uint32_t *pixel = vinfo.framebuffer_addr + vinfo.pitch * i + 4 * i;
                    *pixel = vinfo.color;
                }
                break;
        }
    }

    return; // jump out of kernl_main to scheduler

failure:
    printf("failure\n");
    return;
}
