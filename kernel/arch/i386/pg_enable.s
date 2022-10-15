.globl pg_enable
pg_enable:
  push %ebp
  mov %esp, %ebp
  mov %cr0, %eax
  or $0x80000000, %eax
  mov %eax, %cr0
  mov %ebp, %esp
  pop %ebp
  ret
