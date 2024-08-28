#include "mm/paging.h"
#include "panic.h"

static uint8_t log_page_size;
static uint8_t log_table_size;
static uint64_t* page_bitmap = 0;
static uint64_t page_bitmap_size = 0;
static uint64_t* pml4 = 0;

int kpaging_init(struct earlymem_info info){
    page_bitmap = info.page_bitmap;
    page_bitmap_size = info.bitmap_size;
    pml4 = info.pml4;
    log_page_size = info.log_page_size;
    log_table_size = info.log_table_size;
    return 0;
}

uint64_t alloc_physpage(uint64_t paddr){
    // FIXME search algorithm might not return 0 on failure
    if(page_bitmap == 0 || page_bitmap_size == 0)
        panic("bitmap not initialized");
    if(paddr != 0){
        uint64_t index, bit;
        index = (paddr / (1 << log_page_size)) / 64;
        bit = (paddr / (1 << log_page_size)) % 64;
        if(page_bitmap[index] & (1 << bit)){
            return 0;
        }
        else{
            page_bitmap[index] |= (1 << bit);
            return paddr;
        }
    }

    uint64_t page, width = 64, mask = 0xffffffffffffffff, index, pos = 0;
    for(index = 0; index < page_bitmap_size; index++)
        if(mask != page_bitmap[index])
            break;
    page = ~(page_bitmap[index]);
    pos = __builtin_ffsll(page) - 1;
    page_bitmap[index] |= (1 << pos);
    return (index * 64 + pos) * (1 << log_page_size);
}

inline uint64_t gd_allocpage(){
    uint64_t addr = alloc_physpage(0);
    if(addr == 0 || map_page(addr, addr, PG_WRITABLE) < 0)
        panic("alloc_gdpage failed");
    return addr;
}

void free_page(uint64_t paddr){
    uint64_t index, bit;
    index = (paddr / (1 << log_page_size)) / 64;
    bit = (paddr / (1 << log_page_size)) % 64;
    page_bitmap[index] = page_bitmap[index] & ~(1 << bit);
}

int map_page(uint64_t vaddr, uint64_t paddr, uint64_t flags){
    if(pml4 == 0) // FIXME probably just read cr3
        panic("pml4 not set");

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

int unmap_page(uint64_t vaddr){
    if(pml4 == 0)
        panic("unimplemented");
}

uint64_t create_pagetable(){
    if(pml4 == 0)
        panic("unimplemented");
}
