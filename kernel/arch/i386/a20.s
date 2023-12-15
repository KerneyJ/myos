.globl check_a20
check_a20:
	pushf
	push %bx
	push %dx
	push %di
	push %si

	cli
	mov $0x70, %dx  // CMOS I/O port 0x70 for NMI
	inb %dx, %al // get current value of port 0x70
	push %ax // push current value of port 0x70 to stack

	movb $0x80, %al  # Load 0x80 into AL
	outb %al, %dx // turn off NMI

	xor %ax, %ax	// ax = 0
	mov %ax, %bx

	push %bp
	not %ax			// ax = 0xFFFF
	mov %ax, %bp

	mov $0x0500, %di
	mov $0x0510, %si

	movb (%bx, %di), %al
	push %ax

	movb (%bp, %si), %al
	push %ax

	movb $0x00, (%bx, %di)
	movb $0xFF, (%bp, %si)

	cmpb $0xFF, (%bx, %di)

	pop %ax
	movb %al, (%bp, %si)
	pop %ax
	movb %al, (%bx, %di)

	pop %bp
	pop %cx // pop original CMOS 0x70 port value to cx

	mov $0x0, %ax
	je check_a20__exit
	mov $0x1, %ax

check_a20__exit:
//	pop %si
//	pop %di
//	pop %es
//	pop %ds
//	popf

	push %ax
	mov %cx, %ax
	mov $0x70, %dx  // CMOS I/O port 0x70 for NMI
	outb %al, %dx // reset NMI
	pop %ax
	pop %si
	pop %di
	pop %dx
	pop %bx
	popf
	sti
	ret
