#include <stdint.h>

/* Since we're in 64bit mode each should have 512 entries
 */

#define LOG_PAGE_SIZE 12
#define LOG_TABLE_SIZE 9
#define PG_BIG 0x80
#define PG_PRESENT 0x01
#define PG_WRITABLE 0x02
#define PG_SIZE 1 << (LOG_PAGE_SIZE + LOG_TABLE_SIZE)

extern uint64_t *pml4;
extern uint64_t *pml3;
extern uint64_t *pml2;
uint64_t *page_bitmap = (uint64_t*)(1 << (LOG_TABLE_SIZE + LOG_PAGE_SIZE));

uint64_t init_earlymem();
uint64_t map_page_earlymem(uint64_t vaddr, uint64_t paddr, uint64_t flags);
uint64_t alloc_page_earlymem();
uint64_t free_page_earlymem(uint64_t paddr);
