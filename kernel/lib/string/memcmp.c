#include "string.h"

int memcmp(const void* s1, const void* s2, size_t n){
    unsigned char* ucs1 = (unsigned char*)s1;
	unsigned char* ucs2 = (unsigned char*)s2;
	int r = 0;
	for(size_t i = 0; i < n; ++i)
		r += ucs1[i] - ucs2[i];
	return r;
}
