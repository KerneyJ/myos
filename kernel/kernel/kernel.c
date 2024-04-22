#include "multiboot2.h"
#include "stdio.h"
#include "tty.h"
#include "vm.h"
#include "arch/x86_64/idt.h"

uint32_t breakpoint_calls = 0;

void breakpoint() __attribute__((noinline));
void end() __attribute__((noinline));
void mbt_cmdline(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_blname(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_module(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_bootdev(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_bmeminfo(struct multiboot_tag* tag) __attribute__((noinline));
void mbt_mmap(struct multiboot_tag* tag) __attribute__((noinline));
void* mbt_fb(struct multiboot_tag* tag) __attribute__((noinline));

void breakpoint(){
	breakpoint_calls++;
}

void end(){
	breakpoint_calls++;
}

void mbt_cmdline(struct multiboot_tag* tag){
	printf ("Command line = %s\n", ((struct multiboot_tag_string *) tag)->string);
}

void mbt_blname(struct multiboot_tag* tag){
	printf ("Boot loader name = %s\n", ((struct multiboot_tag_string *) tag)->string);
}

void mbt_module(struct multiboot_tag* tag){
	printf ("Module at 0x%x-0x%x. Command line %s\n",
			((struct multiboot_tag_module *) tag)->mod_start,
			((struct multiboot_tag_module *) tag)->mod_end,
			((struct multiboot_tag_module *) tag)->cmdline);
}

void mbt_bmeminfo(struct multiboot_tag* tag){
	printf ("mem_lower = %uKB, mem_uppe = %uKB\n",
			((struct multiboot_tag_basic_meminfo *) tag)->mem_lower,
			((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);
}

void mbt_bootdev(struct multiboot_tag* tag){
	printf ("Boot device 0x%x,%u,%u\n",
			((struct multiboot_tag_bootdev *) tag)->biosdev,
			((struct multiboot_tag_bootdev *) tag)->slice,
			((struct multiboot_tag_bootdev *) tag)->part);

}

void mbt_mmap(struct multiboot_tag* tag){
	multiboot_memory_map_t *mmap;
	printf ("mmap\n");
	for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
		(multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size;
		mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size)){
		printf (" base_addr = 0x%x%x,"
				" length = 0x%x%x, type = 0x%x\n",
				(unsigned) (mmap->addr >> 32),
				(unsigned) (mmap->addr & 0xffffffff),
				(unsigned) (mmap->len >> 32),
				(unsigned) (mmap->len & 0xffffffff),
				(unsigned) mmap->type);
	}
}

void* mbt_fb(struct multiboot_tag* tag){
	multiboot_uint32_t color;
	unsigned i;
	struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
	void *fb = (void *)tagfb->common.framebuffer_addr;

	switch (tagfb->common.framebuffer_type){
		case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
			{
				unsigned best_distance, distance;
				struct multiboot_color *palette;
				palette = tagfb->framebuffer_palette;
				color = 0;
				best_distance = 4*256*256;
				for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++){
					distance = (0xff - palette[i].blue)
						* (0xff - palette[i].blue)
						+ palette[i].red * palette[i].red
						+ palette[i].green * palette[i].green;

					if (distance < best_distance){
						color = i;
						best_distance = distance;
					}
				}
			}
			break;
		case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
			color = ((1 << tagfb->framebuffer_blue_mask_size) - 1) << tagfb->framebuffer_blue_field_position;
			break;

		case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			color = '\\' | 0x0100;
			break;

		default:
			color = 0xffffffff;
			break;
	}
	breakpoint();

	for (i = 0; i < tagfb->common.framebuffer_width && i < tagfb->common.framebuffer_height; i++){
		switch (tagfb->common.framebuffer_bpp){
			case 8:
				{
					multiboot_uint8_t *pixel = fb + tagfb->common.framebuffer_pitch * i + i;
					*pixel = color;
				}
				break;
			case 15:
			case 16:
				{
					multiboot_uint16_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 2 * i;
					*pixel = color;
				}
				break;
			case 24:
				{
					multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 3 * i;
					*pixel = (color & 0xffffff) | (*pixel & 0xff000000);
				}
				break;
			case 32:
				{
					/* FIXME for some reason this framebuffer address is bad
					 * Exit happens around here
					 *    0x00000000000044fd <+189>:   imul   0x10(%rdi),%ecx
					 *    0x0000000000004501 <+193>:   lea    0x0(,%rdx,4),%eax
					 *    0x0000000000004508 <+200>:   add    %r8,%rcx
					 *    0x000000000000450b <+203>:   mov    %esi,(%rcx,%rax,1)
					 * where r8 stores fb which is eventually moved into rcx; and esi has the color value
					 * the final mov instruction places the color in pixel essentially
					 */
					breakpoint();
					multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 4 * i;
					*pixel = color;
				}
				break;
		}
	}
	return fb;
}


void kernel_main(uint32_t magic, uint32_t addr){
		/*
	 * [+] success
	 * [-] error
	 * [*] info
	 */
	struct multiboot_tag *tag;
	unsigned size;
	void *fb;

	if(magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		goto failure;
//	term_init(); printf("[+]Terminal Initialized\n");

	if (addr & 7) // unaligned multiboot info struct
		goto failure;

	idt_init();

	size = *(unsigned *) addr;
	for (tag = (struct multiboot_tag *) (addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))){
		switch (tag->type){
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				mbt_cmdline(tag); // no triple fault 1st
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				mbt_blname(tag); // no triple fault 2nd
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				mbt_module(tag);
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				mbt_bmeminfo(tag); // no triple fault 4th
				break;
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
				mbt_bootdev(tag); // no triple fault 5th
				break;
			case MULTIBOOT_TAG_TYPE_MMAP:
				mbt_mmap(tag); // no triple fault 3rd
				break;
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
				fb = mbt_fb(tag); // 6th causes triple fault
				break;
		}
	}

	uint16_t* fba = (uint16_t *)fb;
	for(int i = 0; i < 80; i++){
		for(int j = 0; j < 25; j++){
			size_t index = j * 80 + i;
			fba[index] = 'C';
		}
	}
	end();
	printf("Number of calls to breakpoints %i\n", breakpoint_calls);
	return; // jump out of kernl_main to scheduler

failure:
	printf("failure\n");
	return;
//	if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
//		printf("[-]Magic number does not match multiboot magic\n");
//	}
//	pg_init(); printf("[+]Paging Initialized\n");
}
