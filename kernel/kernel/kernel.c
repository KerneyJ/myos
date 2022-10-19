#include "stdio.h"
#include "tty.h" 
#include "vm.h"

void kernel_main(void) 
{
    /* Initialize terminal interface */
    term_init();
	printf("Hello World\n");
	pg_init();
}
