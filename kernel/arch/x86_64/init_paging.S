// Entry Point Declaration
.extern kernel_main
.extern high_kern_end

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
.equ PG_BIG, 0x80
.equ PG_NO_EXEC, 0x8000000000000000
.equ LOG_TABLE_SIZE, 9
.equ LOG_PAGE_SIZE, 12
.equ PAGE_SIZE, (1 << LOG_PAGE_SIZE)
.equ TABLE_SIZE, (1 << LOG_TABLE_SIZE)
.equ STACK_SIZE, 0x1000
.equ STACK_ALIGN, 16

// Bootstrap Stack
.section .bss
.align STACK_ALIGN
.comm boot_stack, STACK_SIZE

// Page Tables
.section .data
.align PAGE_SIZE
.globl boot_p4
boot_p4:
    .quad (boot_p3 + PG_PRESENT + PG_WRITABLE)
    .rept 271
        .quad 0
    .endr
    .quad (boot_p3 + PG_PRESENT + PG_WRITABLE)
    .rept 239
        .quad 0
    .endr

boot_p3:
    .quad (boot_p2 + PG_PRESENT + PG_WRITABLE)
    .rept 511
        .quad 0
    .endr

boot_p2:
    .set pg, 0
    .rept 512
        .quad (pg + PG_PRESENT + PG_BIG + PG_WRITABLE)
        .set pg, pg + (1 << (LOG_TABLE_SIZE + LOG_PAGE_SIZE))
    .endr

// GDT
.globl gdt
gdt:
    .quad 0x0000000000000000
    .quad 0x00AF98000000FFFF
    .quad 0x00CF92000000FFFF
gdt_end:
	.quad 0x0000000000000000

.globl gdtr
gdtr:
    .word gdt_end - gdt - 1
    .quad gdt

// 32-bit GDT
.globl gdt32
gdt32:
    .quad 0x0000000000000000
    .quad 0x00cf9a000000ffff
    .quad 0x00cf92000000ffff

.globl gdtr32
gdtr32:
    .word 23
    .quad gdt32

