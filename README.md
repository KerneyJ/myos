# Build
make ARCH=architecture
architecture options
  - x86\_64
  - i386
# Run
make ARCH=architecture run
# Degbug
target remote :1234
After make ARCH=architecture run-debug
# Notes:
* allocate a stack for each cpu and then point each CPUs stack pointer to those different  
* flush TLB after every page table after context switch
# TODO
[ ] Need to allocate more pages near the framebuffer
[ ] make alloc_physpage give the option to allocate multiple pages
[ ] Need to make a function for switching the color
