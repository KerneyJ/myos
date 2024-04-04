#include "multiboot.h"
#include "stdio.h"
#include "tty.h"
#include "vm.h"
#include "arch.h"

char *msg[25] = {
	"XXXXXXXXXXXXXXXXX      `           XXXXXXXXXXXXXXXXX",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};

void kernel_main(uint32_t magic, uint32_t addr){
	/*
	 * [+] success
	 * [-] error
	 * [*] info
	 */
	term_init(); //printf("[+]Terminal Initialized\n");
//	if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
//		printf("[-]Magic number does not match multiboot magic\n");
//	}
	pg_init(); //printf("[+]Paging Initialized\n");
	printf("              XXXXXXXXXXX               XXXXXXXXXX\n");
	printf("            XX\vOOOOOOOOOOO\rXXX         XXX\vOOOOOOOOOO\rXX\n");
	printf("           XX\vOOOOOOOOOOOOOOO\rXX     XX\vOOOOOOOOOOOOOOO\rXX\n");
	printf("         XX\vOOOOOOOOOOOOOOOOOOO\rXX XX\vOOOOOOOOOOOOOOOOOOO\rXX\n");
	printf("        X\vOOOOOOO       OOOOOOOOO\rX\vOOOOOOOOO       OOOOOOO\rX\n");
	printf("       X\vOOOOOO             OOOOOOOOOOO             OOOOOO\rX\n");
	printf("      X\vOOOOO                  OOOOO                  OOOOO\rX\n");
	printf("      X\vOOOO      \tH A P P Y     \vOOO     \tH A P P Y      \vOOOO\rX\n");
	printf("      X\vOOOO                                           OOOO\rX\n");
	printf("      X\vOOOO      \tV A L E N T I N E ' S   D A Y !      \vOOOO\rX\n");
	printf("       X\vOOO                                           OOO\rX\n");
	printf("        X\vOOOO                \tL O V E                \vOOOO\rX\n");
	printf("         X\vOOOO              \tJ A M I E              \vOOOO\rX\n");
	printf("         X\vOOOOO                                   OOOOO\rX\n");
	printf("          X\vOOOOOO                               OOOOOO\rX\n");
	printf("           X\vOOOOOOOO                         OOOOOOOO\rX\n");
	printf("            X\vOOOOOOOOOO                   OOOOOOOOOO\rX\n");
	printf("             X\vOOOOOOOOOOOO             OOOOOOOOOOOO\rX\n");
	printf("              X\vOOOOOOOOOOOOO         OOOOOOOOOOOOO\rX\n");
	printf("               XX\vOOOOOOOOOOOOOO   OOOOOOOOOOOOOO\rXX\n");
	printf("                 XXX\vOOOOOOOOOOOO OOOOOOOOOOOO\rXXX\n");
	printf("                    XXX\vOOOOOOOOOOOOOOOOOOO\rXXX\n");
	printf("                       XXXX\vOOOOOOOOOOO\rXXXX\n");
	printf("                           XXXX\vOOO\rXXXX\n");
	printf("                               XXX\n");
}
