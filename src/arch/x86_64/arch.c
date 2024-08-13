#include "arch.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/vga.h"
#include "arch/x86_64/early_mem.h"

void configure_arch(struct earlymem_info* info){
	/*TODO
	 * set_interrupts to turn on interrupts just a wrapper around sti
	 * clr_interrupts to turn off interrupts just a wrapper around cli
	 */
	idt_init();
	earlymem_init(info);
	vga_config(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
}
