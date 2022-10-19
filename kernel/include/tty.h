#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

void term_init(void);
void term_setcolor(uint8_t color);
void term_putat(char c, uint8_t color, size_t x, size_t y);
void term_putchar(char c);
void term_newln(void);
void term_wrt(const char* data, size_t size);
void term_wrtstr(const char* data);

#endif
