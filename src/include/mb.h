#include "multiboot2.h"

void* mb_init(uint32_t addr);

void mbt_cmdline(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_blname(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_module(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_bootdev(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_bmeminfo(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_mmap(struct multiboot_tag* tag) __attribute__((noinline));
void* mbt_fb(struct multiboot_tag* tag) __attribute__((noinline));
