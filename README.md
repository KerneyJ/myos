# Build
make ARCH=architecture
architecture options
  - x86\_64
  - i386
# Run
make ARCH=architecture run
# Notes:
* allocate a stack for each cpu and then point each CPUs stack pointer to those different  
* flush TLB after every page table after context switch
# TODO
* make alloc_physpage give the option to allocate multiple pages
