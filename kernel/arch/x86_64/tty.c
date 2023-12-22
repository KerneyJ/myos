#include "arch/i386/vga.h"
#include "tty.h"
#include "string.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t term_row;
size_t term_column;
uint8_t term_color;
uint16_t* term_buffer;

void term_init(void){
	/* Pseudocode for new term init(stolen from Nick
	 * 1) set console width and height
	 * 2) enable VBE (using outw, that's for all of them)
	 * 3) set x resolution
	 * 4) set y resolution
	 * 5) set width
	 * 6) set height
	 * 7) set bits per pixel
	 * 8) set LFB(???)
	 * 9) set index bank to 0
	 * 10) flush vga by writing empty stuff to VGA
	 */
	term_row = 0;
	term_column = 0;
	term_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_GREEN);
	term_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++){
		for(size_t x = 0; x < VGA_WIDTH; x++){
			const size_t index = y * VGA_WIDTH + x;
			term_buffer[index] = vga_entry(' ', term_color);
		}
	}
}

void term_setcolor(uint8_t color){
	term_color = color;
}

void term_putat(char c, uint8_t color, size_t x, size_t y){
	const size_t index = y * VGA_WIDTH + x;
	term_buffer[index] = vga_entry(c, color);
}

void term_putchar(char c){
	if(++term_column == VGA_WIDTH){
		term_column = 0;
		if(++term_row == VGA_HEIGHT)
				term_row = 0;
	}
	if(c != '\n')
		term_putat(c, term_color, term_column, term_row);
	else
		term_newln();
}

void term_newln(void){
	if(term_row > VGA_HEIGHT)
		term_row = 0;
	else
		term_row++;
	term_column = 0;
}

void term_wrt(const char* data, size_t size){
	for(size_t i = 0; i < size; i++)
			term_putchar(data[i]);
}

void term_wrtstr(const char* data){
	term_wrt(data, strlen(data));
}
