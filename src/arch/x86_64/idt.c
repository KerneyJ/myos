#include "arch/x86_64/idt.h"

extern void* isr_stub_table[];
static idt_entry_t idt[256] __attribute__((aligned(0x10)));
static idtr_t idtr;

void idt_init() {
	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1; // 256 max idt entries

	for (uint8_t vector = 0; vector < 32; vector++) {
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
	}

	__asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
}

void clr_interrupt(void) {
	__asm__ volatile ("cli");
}

void set_interrupt(void) {
	__asm__ volatile ("sti");
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
	idt_entry_t* descriptor = &idt[vector];

	descriptor->isr_low			= (uint64_t)isr & 0xFFFF;
	descriptor->kernel_cs		= 0x08;
	descriptor->ist				= 0; // TODO depending on the value of this gate we can get rsp rip rflags error code ss and cs
	descriptor->attributes		= flags;
	descriptor->isr_mid			= ((uint64_t)isr >> 16) & 0xFFFF;
	descriptor->isr_high		= ((uint64_t)isr >> 32) & 0xFFFFFFFF;
	descriptor->reserved		= 0;
}

void exception_handler(ist_t ist /*, cpu_state_t cpu */) {
	__asm__ volatile ("cli; hlt"); // Completely hangs the computer
}
