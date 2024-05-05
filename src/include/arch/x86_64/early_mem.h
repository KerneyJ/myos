#include <stdint.h>

/* Since we're in 64bit mode each should have 512 entries
 */

#define LOG_PAGE_SIZE 12
#define LOG_TABLE_SIZE 9
#define PG_BIG 0x80
#define PG_PRESENT 0x01
#define PG_WRITABLE 0x02

extern uint64_t *pml4;
extern uint64_t *pml3;
extern uint64_t *pml2;

void init_freelist();
uint64_t map_page(uint64_t vaddr, uint64_t paddr, uint64_t flags);
uint64_t alloc_page();
