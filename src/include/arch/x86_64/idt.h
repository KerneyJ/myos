#include <stdint.h>
#include "arch/x86_64/cpu_state.h"

#pragma once

typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t	    ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	uint32_t    isr_high;     // The higher 32 bits of the ISR's address
	uint32_t    reserved;     // Set to zero
} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint64_t ss;
	uint64_t rsp;
	union {
		struct {
			uint64_t carry : 1;
			uint64_t rsrv1 : 1;
			uint64_t parity: 1;
			uint64_t rsrv2 : 1;
			uint64_t auxil : 1;
			uint64_t rsrv3 : 1;
			uint64_t zerof : 1;
			uint64_t signf : 1;
			uint64_t trapf : 1;
			uint64_t intef : 1;
			uint64_t dirct : 1;
			uint64_t overf : 1;
			uint64_t iopl  : 2;
			uint64_t nestt : 1;
			uint64_t rsrv4 : 1;
			uint64_t resmf : 1;
			uint64_t vmmode: 1;
			uint64_t acctl : 1;
			uint64_t vif   : 1;
			uint64_t vip   : 1;
			uint64_t idf   : 1;
			uint64_t rsrv5 : 42;
		} b;
		uint64_t rflag;
	} flags;
	uint64_t cs;
	uint64_t rip;
	uint64_t err;
} __attribute__((packed)) ist_t;

typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;

void idt_init(void);
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void clr_interrupt(void);
void set_interrupt(void);
void exception_handler(ist_t ist, cpu_state_t cpu) __attribute__((noreturn));
