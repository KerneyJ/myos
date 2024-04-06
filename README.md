# Build
make ARCH=architecture
architecture options
  - x86\_64
  - i386
# Run
make run
# Notes:
* allocate a stack for each cpu and then point each CPUs stack pointer to those different  
* flush TLB after every page table after context switch
# Questions
* CLI disables interrupts, how do I turn them back on, or does this just clear interrupts for a certain set of instructions? Until you run STI - Kyle H.
* Can I put the GDT anywhere? Yes - Kyle H.
* ~line 18 descriptor tables header file, what types of descriptors are there
