#include <stdarg.h>
#include "stdio.h"
#include "tty.h"

static char digits[] = "0123456789ABCDEF";
void printnum(int n, int base){
    int i = 0;
	char buf[16];
	do{
      buf[i++] = digits[n % base];
	}while((n /= base) != 0 && i < 16);
	
	while(--i >= 0)
		term_putchar(buf[i]);
}

void printf(const char* restrict format, ...){
    va_list params;
	va_start(params, format);
	while(*format != '\0'){
		if(*format == '%'){
		    format++;
			switch(*format){
				case 'd':
				case 'i':
				    printnum(va_arg(params, int), 10);
				    break;
				case 'x':
					term_putchar('0');
					term_putchar('x');
					printnum(va_arg(params, int), 16);
					break;
			}
		}
        else if(*format == '\n')
		    term_newln();
		else
		    term_putchar(*format);		
	    format++;
	}
}
