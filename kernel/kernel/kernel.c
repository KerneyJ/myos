#include "printf.h"
#include "tty.h" 
#include "vm.h"

void kernel_main(void) 
{
    /* Initialize terminal interface */
    term_init();
	pg_init();
	printf("Hello World! %i\n", 45);
}
