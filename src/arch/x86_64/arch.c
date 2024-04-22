#include "arch.h"
#include "arch/x86_64/idt.h"

void configure_arch(void){
	/*TODO
	 * set_interrupts to turn on interrupts just a wrapper around sti
	 * clr_interrupts to turn off interrupts just a wrapper around cli
	 */
	idt_init();
}
