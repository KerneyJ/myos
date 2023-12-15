#include "string.h"

void* memset(void* ptr, int c, size_t s){
    unsigned char* ucptr = (unsigned char*)ptr;
    for(size_t i = 0; i < s; ++i)
		ucptr[i] = c;
	return ptr;
}
