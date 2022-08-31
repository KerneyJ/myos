#include "tty.h" 

void kernel_main(void) 
{
    /* Initialize terminal interface */
    terminal_initialize();
    terminal_writestring("Hello World!\n");
	terminal_writestring("Something to prove that it compiles\n");
}
