#ifndef MB_H
#define MB_H
#include <stdint.h>
#include "multiboot2.h"
#include "video.h"
#include "stdio.h"

int mb_init(uint32_t addr, struct video_info *vinfo);

void mbt_cmdline(struct multiboot_tag* tag);
void mbt_blname(struct multiboot_tag* tag);
void mbt_module(struct multiboot_tag* tag);
void mbt_bootdev(struct multiboot_tag* tag);
void mbt_bmeminfo(struct multiboot_tag* tag); 
void mbt_mmap(struct multiboot_tag* tag);
void mbt_fb(struct multiboot_tag* tag, struct video_info* vinfo);

#endif
