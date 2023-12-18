.code32
.globl pgdir_load
pgdir_load:
  push %ebp
  mov %esp, %ebp
  mov 8(%esp), %eax
  mov %eax, %cr3
  mov %ebp, %esp
  pop %ebp
  ret
