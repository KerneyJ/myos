// outw port, value
.global vga_config
vga_config:
	// %edi has width, %esi has height
	// mov $0x01CE, %edx // VBE index port
	// mov $0x01CF, %eax // VBE data port
	push %rax
	push %rdx

	// Disable VBE
	mov $0x01CE, %ax
	outw %ax, $0x4 // 4 == vbe enable display index
	mov $0x01CF, %ax
	outw %ax, $0x0 // write to that index disable

	// Set X Resolution
	mov $0x01CE, %ax
	outw %ax, $0x1
	mov $0x01CF, %ax
	mov %di, %dx
	outw %ax, %dx

	// Set Y Resolution
	mov $0x01CE, %ax
	outw %ax, $0x2
	mov $0x01CF, %ax
	mov %si, %dx
	outw %ax, %dx

	// Set Width
	mov $0x01CE, %ax
	outw %ax, $0x6
	mov $0x01CF, %ax
	mov %di, %dx
	outw %ax, %dx

	// Set Height
	mov $0x01CE, %ax
	outw %ax, $0x7
	mov $0x01CF, %ax
	mov $0x1000, %dx
	outw %ax, %dx

	// Set bits per pixel
	mov $0x01CE, %ax
	outw %ax, $0x3
	mov $0x01CF, %ax
	outw %ax, $0x20

	// Set linear frame buffer and enable VBE
	mov $0x01CE, %ax
	outw %ax, $0x4
	mov $0x01CF, %ax
	outw %ax, $0x41

	// Disable Bank mode
	mov $0x01CE, %ax
	outw %ax, $0x5
	mov $0x01CF, %ax
	outw %ax, $0x0

	pop %rdx
	pop %rax
	ret
