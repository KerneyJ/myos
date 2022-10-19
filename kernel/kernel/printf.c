#include <stdarg.h>
#include "printf.h"
#include "tty.h"

static char digits[] = "0123456789ABCDEF";
void printnum(int n){
    int i = 0;
	char buf[16];
	do{
      buf[i++] = digits[n % 10];
	}while((n /= 10) != 0 && i < 16);
	
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
				    printnum(va_arg(params, int));
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
