#include "arch/x86_64/early_mem.h"
#include "panic.h"

/* from 0x200000 + 6 * PAGE_SIZE to 0x400000 + 6 * PAGE_SIZE 
 * Maps tracks first 512 GiB in of address space*/
uint64_t *page_bitmap = (uint64_t *)(MEM_BASE + 6 * PAGE_SIZE);
uint64_t page_bitmap_size = 0;
uint64_t *earlymem_pt = 0;

uint64_t earlymem_init(){
    // Zero out the bitmap
    page_bitmap_size = BITMAP_ENTRIES * BITMAP_ENTRY_SIZE; 
    for(uint64_t i = 0; i < page_bitmap_size; i++)
        page_bitmap[i] = 0;

    // Mark up to 0x800000 as used in bitmap
    for(int i = 0; i < 32; i++)
        page_bitmap[i] = 0xffffffffffffffff;

    // pre-allocate a page table for future page tables
    // This maps from 0x600000 to 0x7fffff
    uint64_t* pd =  (uint64_t*)(&pml2);
    uint64_t* pt = page_bitmap + page_bitmap_size; // right after the bitmap
    for(int i = 0; i < TABLE_SIZE; i++)
        pt[i] = 0; // zero out every entry;
    pd[3] = (uint64_t)pt + PG_PRESENT + PG_WRITABLE;
    earlymem_pt = pt;

    // map page for the framebuffer
    uint64_t fb_addr = 0xfd000000, paddr;
    for(int i = 0; i < 32; i++){
        paddr = alloc_page_earlymem(fb_addr + (i * PAGE_SIZE));
        if(paddr == 0)
            panic();
        map_page_earlymem(fb_addr + (i * PAGE_SIZE), paddr, PG_WRITABLE);
    }

    return 0;
}

uint64_t alloc_pagetable_earlymem(){
    if(earlymem_pt == 0)
        return 0; // in future panic

    uint64_t* pt_addr;
    for(int i = 0; i < TABLE_SIZE; i++){
        if(!(earlymem_pt[i] & PG_PRESENT)){
            pt_addr = (uint64_t*)((i * PAGE_SIZE) + EARLY_PT_BASE);
            earlymem_pt[i] = (uint64_t)pt_addr + PG_PRESENT + PG_WRITABLE;
            for(int j = 0; j < TABLE_SIZE; j++)
                pt_addr[j] = 0;
            return pt_addr;
        }
    }
    return 0; // in future panic
}

uint64_t alloc_page_earlymem(uint64_t addr){
    /* In future maybe have static variable or smth that caches
     * the location of free pages
     * or have some sort of data structure to make this faster
     */
    // find a free page
    if(addr != 0){
        uint64_t index, bit;
        index = (addr / PAGE_SIZE) / 64;
        bit = (addr / PAGE_SIZE) % 64;
        if(page_bitmap[index] & (1 << bit))
            return 0;
        else{
            page_bitmap[index] |= bit;
            return addr;
        }
    }
    uint64_t page, width = 64, mask = 0xffffffffffffffff, index, pos = 0;
    for(index = 0; index < 32768; index++){
        if(mask != page_bitmap[index]){
            break;
        }
    }
    page = page_bitmap[index];
    while(width > 1){
        if((mask & page) != mask){
            width /= 2;
            mask = mask & (mask >> width);
        }
        else{
            mask = mask << width;
            pos += width;
        }
    }
    if((mask & page) == mask){
        mask <<= 1;
        pos -= 1;
    }

    // mark that page as used
    page_bitmap[index] |= mask;

    return (index * 64 + pos) * PAGE_SIZE; // the base physical address of the page
}

uint64_t free_page_earlymem(uint64_t paddr){
	uint64_t index, bit;
	index = (paddr / PAGE_SIZE) / 64;
	bit = (paddr / PAGE_SIZE) % 64;
	page_bitmap[index] = page_bitmap[index] & ~(1 << bit);
	return 0;
}

uint64_t map_page_earlymem(uint64_t vaddr, uint64_t paddr, uint64_t flags){
    // need to rework this make it more modular
    uint64_t pml4_index = (vaddr >> 39) & 0x1ff;
    uint64_t pml3_index = (vaddr >> 30) & 0x1ff;
    uint64_t pml2_index = (vaddr >> 21) & 0x1ff;
    uint64_t pml1_index = (vaddr >> 12) & 0x1ff;
    uint64_t *pml;

    pml = (uint64_t*)&pml4;
    if(!(pml[pml4_index] & PG_PRESENT))
        return -1; // means we are trying to allocate memory very high in the address space say no for now, but maybe later

    pml = (uint64_t*)(pml[pml4_index] & (~0xfff));
    if(!(pml[pml3_index] & PG_PRESENT)){
        uint64_t* pml2 = alloc_pagetable_earlymem();
        uint64_t* pml1 = alloc_pagetable_earlymem();
        pml[pml3_index] = (uint64_t)pml2 + PG_WRITABLE + PG_PRESENT;
        pml2[pml2_index] = (uint64_t)pml1 + PG_WRITABLE + PG_PRESENT;
        pml1[pml1_index] = paddr + PG_PRESENT + (flags & 0xfff);
        return 0;
    }

    pml = (uint64_t*)(pml[pml3_index] & (~0xfff)); // FIXME check if for PG_BIG Flag
    if(!(pml[pml2_index] & PG_PRESENT)){
        uint64_t* pml1 = alloc_pagetable_earlymem();
        pml[pml2_index] = (uint64_t)pml1 + PG_WRITABLE + PG_PRESENT;
        pml1[pml1_index] = paddr + PG_PRESENT + (flags & 0xfff);
        return 0;
    }

    pml = (uint64_t*)(pml[pml2_index] & (~0xfff));
    if(!(pml[pml1_index] & PG_PRESENT)){
        pml[pml1_index] = paddr + PG_PRESENT + (flags & 0xfff);
        return 0;
    }
    else{
        // FIXME gotta do something cause that means this page is already mapped
        return -1;
    }
}
