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
- [ ] Need to finish MM
    - [ ] implement alloc_pagetable
    - [ ] implement unmap_page
    - [ ] implement create_block
- [ ] Need to allocate more pages near the framebuffer
- [ ] make alloc_physpage give the option to allocate multiple pages
- [ ] Need to make a function for switching the color

## Train of thought(05/07/2025)
Goal is to fix bug where drawing a line causes a page fault(write to 0xfd020068), this is because I only map the first 32 pages after 0xfd000000(framebuffer address)
* Current make ARCH=x86_64 produces a lot of wacky warnings, need to fix those
* After fixing most of those warnings need to pre-allocate initial pool of page tables(in kpaging_init)
* After allocating initial pool of pagetables need to implement alloc_pagetable such that it uses that initial pool
  and is otherwise self sufficient(it can create pagetables by itself)
* After implementing alloc_pagetable replace calls to alloc_pagetable_earlymem in map_page with alloc_pagetbale
* After proceed to video memory
  * here start by allocating more memory for the framebuffer(right now its 32 pages which is not enough)
