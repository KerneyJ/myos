#include <stdint.h>

#define PG_SIZE			4096
#define PTE_PRESENT		0x001
#define PTE_RW			0x002
#define PTE_USER		0x004
#define PTE_PWT			0x008

void pg_init(void);
void pgdir_init(void);
void pgtbl_init(void);
