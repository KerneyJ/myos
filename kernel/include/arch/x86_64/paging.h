#define PG_SIZE			0x1000
#define TABLE_SIZE		512
#define PTE_PRESENT		0x001
#define PTE_RW			0x002
#define PTE_USER		0x004
#define PTE_PWT			0x008
#define INIT_PML4_ADDR	0xA00000
#define INIT_PML3_ADDR	INIT_PML4_ADDR + PG_SIZE
#define INIT_PGD_ADDR	INIT_PML3_ADDR + PG_SIZE
#define INIT_PGTB_ADDR	INIT_PGD_ADDR + PG_SIZE

//void pg_init(void);
//void pgdir_init(void);
//void pgtbl_init(void);
