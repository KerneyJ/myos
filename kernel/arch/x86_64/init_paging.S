// Entry Point Declaration
.extern kernel_main
.extern high_kern_end



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
