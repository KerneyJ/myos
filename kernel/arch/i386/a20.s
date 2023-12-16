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

.globl enable_a20
enable_a20:
	cli

	call a20wait
	mov $0xAD, %al
	out %al, $0x64

	call a20wait
	mov $0xD0, %al
	out %al, $0xD0

	call a20wait2
	in $0x60, %al
	push %ax

	call a20wait
	mov $0xD1, %al
	out %al, $0x64

	call a20wait
	pop %ax
	or $0x2, %al
	out %al, $0x60

	call a20wait
	mov $0xAE, %al
	out %al, $0x64

	call a20wait
	sti
	ret

a20wait:
	in $0x64, %al
	test $0x02, %al
	jnz a20wait
	ret

a20wait2:
	in $0x64, %al
	test $0x01, %al
	jz a20wait2
	ret
