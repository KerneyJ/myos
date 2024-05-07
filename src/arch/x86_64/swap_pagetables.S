.section .text
.global swap_pagetables
swap_pagetables:
	movq %rdi, %rax
	movq %rax, %cr3
	ret
