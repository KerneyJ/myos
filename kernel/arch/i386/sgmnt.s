.globl setup_segment
setup_segment:
	xor %ax, %ax
	mov %ax, %ds
	mov %ax, %es
//	mov %di, %ax // in boot.s stack_bottom is placed in di
//	mov %ax, %ss // I guess just don't initialize the stack segment because that causes a triple fault
	ret
