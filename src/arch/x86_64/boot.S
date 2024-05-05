// Constants and Definitions
.equ KERNEL_VMA, 0xffff880000000000
.equ MSR_EFER, 0xC0000080
.equ EFER_LM, 0x100
.equ EFER_NX, 0x800
.equ EFER_SCE, 0x001
.equ CR0_PAGING, 0x80000000
.equ CR4_PAE, 0x20
.equ CR4_PSE, 0x10
.equ PG_PRESENT, 0x1
.equ PG_WRITABLE, 0x2
.equ PG_USER, 0x4
.equ PG_BIG, 0x80 // For 2 MiB pages
.equ PG_NO_EXEC, 0x8000000000000000
.equ LOG_TABLE_SIZE, 9
.equ LOG_PAGE_SIZE, 12
.equ PAGE_SIZE, (1 << LOG_PAGE_SIZE)
.equ TABLE_SIZE, (1 << LOG_TABLE_SIZE)
.equ STACK_SIZE, 0x1000
.equ STACK_ALIGN, 16
.equ FRAMEBUFFER_WIDTH, 80
.equ FRAMEBUFFER_HEIGHT, 25

.section .data
.code64
.align PAGE_SIZE
.global pml4
pml4:
	.quad (pml3 + PG_PRESENT + PG_WRITABLE)
	.rept 271
		.quad 0
	.endr
	.quad (pml3 + PG_PRESENT + PG_WRITABLE)
	.rept 239
		.quad 0
	.endr

pml3: // 512 GiB
	.quad (pml2 + PG_PRESENT + PG_WRITABLE)
	.rept 511
		.quad 0
	.endr

pml2: /* with big pages this maps 1 GiB */
	.quad (PG_PRESENT + PG_BIG + PG_WRITABLE)
	.rept 511
		.quad 0
	.endr

.section multiboot
.align 8
.global mbheader
mbheader:
	.long 0xe85250d6
	.long 0
	.long .hdr_end - mbheader
	.long -(0xe85250d6 + 0 + (.hdr_end - mbheader))

	/* BEGIN TAGS */

	/* framebufer */
		framebuffer_tag_start:
			.word 5
			.word 0
			.long 20 # framebuffer_tag_end - framebuffer_tag_start
			.long FRAMEBUFFER_WIDTH
			.long FRAMEBUFFER_HEIGHT
			.long 32
		framebuffer_tag_end:


	/* tags end */
	.long 0, 0
	.long 8
.hdr_end:

/*
The multiboot standard does not define the value of the stack pointer register
(esp) and it is up to the kernel to provide a stack. This allocates room for a
small stack by creating a symbol at the bottom of it, then allocating 16384
bytes for it, and finally creating a symbol at the top. The stack grows
downwards on x86. The stack is in its own section so it can be marked nobits,
which means the kernel file is smaller because it does not contain an
uninitialized stack. The stack on x86 must be 16-byte aligned according to the
System V ABI standard and de-facto extensions. The compiler will assume the
stack is properly aligned and failure to align the stack will result in
undefined behavior.
*/
.section .bss
.align STACK_ALIGN
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .data
.global gdt
gdt:
	.quad 0x0000000000000000
	.quad 0x00AF98000000FFFF
	.quad 0x00CF92000000FFFF
gdt_end:
	.quad 0x0000000000000000

.global gdtr
gdtr:
	.word gdt_end - gdt - 1
	.quad gdt

// 32-bit GDT
.global gdt32
gdt32:
	.quad 0x0000000000000000
	.quad 0x00cf9a000000ffff
	.quad 0x00cf92000000ffff

.global gdtr32
gdtr32:
	.word 23
	.quad gdt32


/*
The linker script specifies _start as the entry point to the kernel and the
bootloader will jump to this position once the kernel has been loaded. It
doesn't make sense to return from this function as the bootloader is gone.
*/
.code32
.section .text
.global _start
.type _start, @function
_start:
	cli

	movl %eax, %edi
	movl %ebx, %esi

	/* Enable PAE and PSE
	*/
	movl %cr4, %eax
	orl $(1 << 5), %eax /* PAE */
	orl $(1 << 4), %eax /* PSE */
	movl %eax, %cr4

	/* Enable long mode and the NX bit
	*/
	movl $0xC0000080, %ecx
	rdmsr
	orl $(0x100 | 0x800 | 0x001), %eax
	wrmsr

	/* Set cr3 to p4 of the boot page table
	*/
	movl $pml4, %eax
	movl %eax, %cr3

	/* Enable paging */
	mov %cr0, %eax
	orl $0x80000000, %eax
	mov %eax, %cr0

	/* Load the gdt */
	movl $gdtr, %eax
	lgdt (%eax)

	movl $0x10, %eax
	movw %ax, %ds
	movw %ax, %ss

	ljmp $0x8, $.trampoline

.code64
.trampoline:
	movq $.next_ptr, %rax
	movq (%rax), %rax
	jmp *%rax

.next_ptr:
	.quad .next

.next:
	movq (gdtr + 2), %rax
	movabs $0xffff880000000000, %rbx
	addq %rbx, %rax
	movq %rax, (gdtr + 2)
	movq $gdtr, %rax
	addq %rbx, %rax
	lgdt (%rax)

	/*
	This is a good place to initialize crucial processor state before the
	high-level kernel is entered. It's best to minimize the early
	environment where crucial features are offline. Note that the
	processor is not fully initialized yet: Features such as floating
	point instructions and instruction set extensions are not initialized
	yet. The GDT should be loaded here. Paging should be enabled here.
	C++ features such as global constructors and exceptions will require
	runtime support to work as well.
	*/

	/*
	Enter the high-level kernel. The ABI requires the stack is 16-byte
	aligned at the time of the call instruction (which afterwards pushes
	the return pointer of size 4 bytes). The stack was originally 16-byte
	aligned above and we've pushed a multiple of 16 bytes to the
	stack since (pushed 0 bytes so far), so the alignment has thus been
	preserved and the call is well defined.
	*/

	/* To set up a stack, we set the esp register to point to the top of the
	stack (as it grows downwards on x86 systems). This is necessarily done
	in assembly as languages such as C cannot function without a stack.
	*/
	movq $stack_top, %rsp
	movq %rsp, %rbp

	call kernel_main

	/*
	If the system has nothing more to do, put the computer into an
	infinite loop. To do that:
	1) Disable interrupts with cli (clear interrupt enable in eflags).
	   They are already disabled by the bootloader, so this is not needed.
	   Mind that you might later enable interrupts and return from
	   kernel_main (which is sort of nonsensical to do).
	2) Wait for the next interrupt to arrive with hlt (halt instruction).
	   Since they are disabled, this will lock up the computer.
	3) Jump to the hlt instruction if it ever wakes up due to a
	   non-maskable interrupt occurring or due to system management mode.
	*/
.panic:
	cli
	hlt
	jmp .panic
 
/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start
