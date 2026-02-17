#include "mb.h"

int mb_init(uint32_t addr, struct video_info *vinfo){
    struct multiboot_tag *tag;
    for (tag = (struct multiboot_tag *) (addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))){
        switch (tag->type){
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                mbt_cmdline(tag);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                mbt_blname(tag);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                mbt_module(tag);
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                mbt_bmeminfo(tag);
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                mbt_bootdev(tag);
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                mbt_mmap(tag);
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                mbt_fb(tag, vinfo);
                break;
        }
    }
    return 0;
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

void mbt_fb(struct multiboot_tag* tag, struct video_info* vinfo){
    uint32_t color;
	unsigned i;
	struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
	void *fb = (void *)tagfb->common.framebuffer_addr;

    // set up the color
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
    vinfo->color = color;
    vinfo->framebuffer_addr = fb;
    vinfo->bpp = tagfb->common.framebuffer_bpp;
    vinfo->width = tagfb->common.framebuffer_width;
    vinfo->height = tagfb->common.framebuffer_height;
    vinfo->pitch = tagfb->common.framebuffer_pitch;
}

