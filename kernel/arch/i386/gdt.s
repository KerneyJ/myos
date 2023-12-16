.globl setup_gdt
setup_gdt:
	cli
	pusha
	lgdt [toc]
	sti
	popa
	ret

gdt_data:
	.int 0 				// null descriptor
	.int 0
 
//	gdt code:				// code descriptor
	.short 0xFFFF			// limit low
	.short 0				// base low
	.byte 0					// base middle
	.byte 0b10011010 		// access
	.byte 0b11001111 		// granularity
	.byte 0					// base high

//	gdt data:				// data descriptor
	.short 0xFFFF			// limit low (Same as code)
	.short 0				// base low
	.byte 0					// base middle
	.byte 0b10010010 		// access
	.byte 0b11001111 		// granularity
	.byte 0					// base high

end_of_gdt:
toc:
	.short end_of_gdt - gdt_data - 1 	// limit (Size of GDT)
	.int gdt_data				// base of GDT
